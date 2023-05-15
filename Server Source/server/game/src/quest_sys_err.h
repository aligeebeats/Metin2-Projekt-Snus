#undef sys_err

#ifdef _WIN32
#define sys_err(fmt, ...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#else
#define sys_err(fmt, args...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, ##args)
#endif
