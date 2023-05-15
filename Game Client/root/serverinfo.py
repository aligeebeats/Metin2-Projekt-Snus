import app

app.ServerName = None

STATE_NONE = '...'
		
STATE_DICT = {
	0 : '....',
	1 : 'NORM',
	2 : 'BUSY',
	3 : 'FULL'	}

SERVER1_CHANNEL_DICT = {
	1:{'key':11,'name':'Channel1','ip':'127.0.0.1','tcp_port':13001,'udp_port':13001,'state':STATE_NONE,},
	2:{'key':12,'name':'Channel2','ip':'127.0.0.1','tcp_port':13002,'udp_port':13002,'state':STATE_NONE,},
}

SERVER2_CHANNEL_DICT = {
	1:{'key':21,'name':'Channel1','ip':'192.168.178.43','tcp_port':13001,'udp_port':13001,'state':STATE_NONE,},
}

# SERVER3_CHANNEL_DICT = {
	# 1:{'key':31,'name':'Channel1','ip':'192.168.0.160','tcp_port':13001,'udp_port':13001,'state':STATE_NONE,},
	# 2:{'key':32,'name':'Channel2','ip':'192.168.0.160','tcp_port':13002,'udp_port':13002,'state':STATE_NONE,},
	# 3:{'key':33,'name':'Channel3','ip':'192.168.0.160','tcp_port':13003,'udp_port':13003,'state':STATE_NONE,},
# }

REGION_NAME_DICT = {
	0 : 'GERMANY',
}

REGION_AUTH_SERVER_DICT = {
	0 : {
		1 : { 'ip':'127.0.0.1', 'port':11002, }, 
		2 : { 'ip':'192.168.178.43', 'port':11002, },
		3 : { 'ip':'192.168.0.160', 'port':11002, },
		}	
}

REGION_DICT = {
	0 : {
		1 : { 'name' : 'Windows', 'channel' : SERVER1_CHANNEL_DICT, },
		2 : { 'name' : 'Root', 'channel' : SERVER2_CHANNEL_DICT, },
		# 3 : { 'name' : 'FreeBSD Lokal', 'channel' : SERVER3_CHANNEL_DICT, },
		},
}

MARKADDR_DICT = {
	10 : { 'ip' : '127.0.0.1', 'tcp_port' : 13001, 'mark' : '10.tga', 'symbol_path' : '10', },
	20 : { 'ip' : '192.168.178.43', 'tcp_port' : 13001, 'mark' : '10.tga', 'symbol_path' : '10', },
	30 : { 'ip' : '192.168.0.160', 'tcp_port' : 13001, 'mark' : '10.tga', 'symbol_path' : '10', },
	}

TESTADDR = { 'ip' : '127.0.0.1', 'tcp_port' : 50000, 'udp_port' : 50000, }

#DONE
