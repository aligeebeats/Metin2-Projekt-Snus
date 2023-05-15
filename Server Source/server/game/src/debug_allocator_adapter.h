#pragma once
#include <ctime>
#include <iostream>
#include <fstream>
#include <boost/unordered_map.hpp>
#define DBGALLOC_LOG_FILENAME "dbgalloc.log"
#define DBGALLOC_REPORT_FILENAME "dbgalloc_report.log"

struct AllocTag {
	AllocTag(const char* file, size_t line) : in_use(true), age(1) {
		this->file = file;
		this->line = line;
	}
	void Reuse(const char* file, size_t line) {
		in_use = true;
		this->file = file;
		this->line = line;
		age = IncreaseAge(age);
	}
	void Unuse(const char* file, size_t line) {
		in_use = false;
		this->file = file;
		this->line = line;
		age = IncreaseAge(age);
	}
	static size_t IncreaseAge(size_t value) {
		size_t result = value;
		++result;
		if (result == 0) {
			result = 1;
		}
		return result;
	}
	bool in_use;
	const char* file;
	size_t line;
	size_t age;
};

inline std::ostream& operator<<(std::ostream& os, const AllocTag tag) {
	return os << "<" << tag.file << " line:" << tag.line <<
		" age:" << tag.age << ">";
}

struct ScopedOutputFile {
	ScopedOutputFile(const char* filename,
		std::ios_base::openmode mode = std::ios_base::app) {
		stream.open(filename, mode);
	}
	~ScopedOutputFile() {
		if (stream.is_open()) {
			stream.close();
		}
	}
	std::ostream& Datetime() {
		char buf[24];
		time_t t = ::time(NULL);
		::strftime(buf, 24, "%Y-%m-%d %H:%M:%S", ::localtime(&t));
		return stream << buf;
	}
	std::ofstream stream;
};

template<typename T> class DebugPtr;

template<class Detail>
class DebugAllocatorAdapter {
public:
	~DebugAllocatorAdapter() {}

	static void StaticSetUp() {
		GetInstance().detail_.SetUp();
	}

	static void StaticTearDown() {
		DebugAllocatorAdapter& instance = GetInstance();
		instance.DumpLeakReport();
		instance.detail_.TearDown();
	}

	static void* Alloc(size_t size) {
		return GetInstance().detail_.Alloc(size);
	}

	static void Free(void* p) {
		GetInstance().detail_.Free(p);
	}

	static size_t MarkAcquired(void* p, const char* file, size_t line, const char* context) {
		if (p == NULL) {
			return 0;
		}
		AllocMapType& alloc_map = GetInstance().alloc_map_;
		size_t age = 0;
		auto it = alloc_map.find(p);
		if (it == alloc_map.end()) {
			AllocTag tag(file, line);
			alloc_map.insert(AllocMapType::value_type(p, tag));
			age = tag.age;
		} else {
			AllocTag& tag = it->second;
			if (!tag.in_use) {
				tag.Reuse(file, line);
				age = tag.age;
			} else {
				ScopedOutputFile of(DBGALLOC_LOG_FILENAME);
				if (of.stream.is_open()) {
					of.Datetime() << " [" << context << "] " <<
						p << " " << tag << " already in use. " <<
						"(" << file << " line:" << line << ")" <<
						std::endl;
				}
			}
		}
		return age;
	}

	template<typename T>
	static T* MarkReleased(T* p, const char* file, size_t line, const char* context) {
		return (GetInstance().VerifyReference(p, file, line, context, true) ? p : NULL);
	}
    template<typename T>
    static T* MarkReleased(DebugPtr<T>& ptr, const char* file, size_t line, const char* context) {
		return (GetInstance().VerifyReference(ptr.Get(), file, line, context, true, true, ptr.GetAge()) ? ptr.Get() : NULL);
    }

	static size_t RetrieveAge(void* p) {
		if (p == NULL) {
			return 0;
		}
		AllocMapType& alloc_map = GetInstance().alloc_map_;
		auto it = alloc_map.find(p);
		if (it != alloc_map.end()) {
			AllocTag& tag = it->second;
			if (tag.in_use) {
				return tag.age;
			}
		}
		return 0;
	}

	template<typename T>
	static T* Verify(T* p, size_t age, const char* file = NULL, size_t line = 0) {
		return (GetInstance().VerifyReference(p, file, line, "ref", false, true, age) ? p : NULL);
	}

	template<typename T>
	static T* VerifyDeletion(T* p, const char* file, size_t line, bool verify_age = false, size_t age = 0) {
		return (GetInstance().VerifyReference(p, file, line, "pre_delete", false, verify_age, age) ? p : NULL);
	}

	static void LogBoundaryCorruption(void* p, size_t age) {
		AllocMapType& alloc_map = GetInstance().alloc_map_;
		auto it = alloc_map.find(p);
		if (it != alloc_map.end()) {
			AllocTag& tag = it->second;
			ScopedOutputFile of(DBGALLOC_LOG_FILENAME);
			if (of.stream.is_open()) {
				of.Datetime() << " [boundary] " <<
					p << " " << tag << " age header corrupted:" <<
					" current age value " << age << std::endl;
				PrintStack( of.stream );
			}
		}
	}

private:
	DebugAllocatorAdapter() {}

	static DebugAllocatorAdapter& GetInstance() {
		static DebugAllocatorAdapter instance;
		return instance;
	}

	bool VerifyReference(void* p, const char* file, size_t line,
			const char* context, bool mark_released,
			bool verify_age = false, size_t age = 0) {
		if (p == NULL) {
			return mark_released;
		}
		auto it = alloc_map_.find(p);
		if (it != alloc_map_.end()) {
			AllocTag& tag = it->second;
			if (tag.in_use) {
				if (verify_age && tag.age != age) {
					ScopedOutputFile of(DBGALLOC_LOG_FILENAME);
					if (of.stream.is_open()) {
						of.Datetime() << " [" << context << "] " <<
							p << " " << tag << " has different age with " <<
							age << " (" << file << " line:" << line << ")" <<
							std::endl;
					}
				} else {
					if (mark_released) {
						tag.Unuse(file, line);
					}
					return true;
				}
			} else {
				ScopedOutputFile of(DBGALLOC_LOG_FILENAME);
				if (of.stream.is_open()) {
					of.Datetime() << " [" << context << "] " <<
						p << " " << tag << " already freed. " <<
						"(" << file << " line:" << line << ")" <<
						std::endl;
				}
			}
		} else {
			ScopedOutputFile of(DBGALLOC_LOG_FILENAME);
			if (of.stream.is_open()) {
				of.Datetime() << " [" << context << "] " <<
					p << " is not a valid entry. " <<
					"(" << file << " line:" << line << ")" <<
					std::endl;
			}
		}
		return false;
	}

	void DumpLeakReport() {
		auto it = alloc_map_.begin(), end = alloc_map_.end();
		for ( ; it != end; ++it) {
			if (it->second.in_use) {
				break;
			}
		}
		if (it == end) {
			return;
		}
		ScopedOutputFile of(DBGALLOC_REPORT_FILENAME);
		if (!of.stream.is_open()) {
			return;
		}
		of.Datetime() << std::endl;
		for ( ; it != end; ++it) {
			AllocTag& tag = it->second;
			if (tag.in_use) {
				of.stream << "[leak] " << it->first << " " << tag << std::endl;
			}
		}
	}

	typedef boost::unordered_map<void*, AllocTag> AllocMapType;

	Detail detail_;
	AllocMapType alloc_map_;
};
