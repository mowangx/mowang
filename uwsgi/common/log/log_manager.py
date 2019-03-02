# -*- coding: utf-8 -*-


try:  # noqa
	import AsyncLogManager
	LogManager = AsyncLogManager.AsyncLogManager
	SALogger = AsyncLogManager.SALogger
	CRITICAL = AsyncLogManager.CRITICAL
	ERROR = AsyncLogManager.ERROR
	WARNING = AsyncLogManager.WARN
	WARN = AsyncLogManager.WARN
	INFO = AsyncLogManager.INFO
	DEBUG = AsyncLogManager.DEBUG

	STREAM = AsyncLogManager.STREAM
	SYSLOG = AsyncLogManager.SYSLOG
	FILE = AsyncLogManager.FILE

	AsyncLogManager.redirect_stdio_to_async_logger()
	print "Using AsyncLogManager"
except:
	print "Using PythonLogManager"
	# python 的logging模块进行简单封装
	import new
	import sys
	import logging
	import logging.handlers as LH
	import time
	import traceback
	import platform

	import json

	def compact_traceback():
		t, v, tb = sys.exc_info()
		tbinfo = []
		if tb is None:
			return
		while tb:
			tbinfo.append((
				tb.tb_frame.f_code.co_filename,
				tb.tb_frame.f_code.co_name,
				str(tb.tb_lineno)
			))
			tb = tb.tb_next

		# just to be safe
		del tb

		pfile, function, line = tbinfo[-1]
		info = ' '.join(['[%s|%s|%s]' % x for x in tbinfo])
		return (pfile, function, line), t, v, info

	def log_compact_traceback(self):
		self.error(traceback.format_exc())

	# log级别定义，从高到低
	CRITICAL = logging.CRITICAL
	ERROR = logging.ERROR
	WARNING = logging.WARN
	WARN = logging.WARN
	INFO = logging.INFO
	DEBUG = logging.DEBUG

	# 日志输出流
	STREAM 	= "stream"
	SYSLOG 	= "syslog"
	FILE	= "file"
	CUSTOME	= "custom"

	# 产生一个logger对象,最终应该按照配置文件来初始化自己
	class LogManager(object):
		created_modules = set()
		log_level = DEBUG
		log_handle = STREAM
		log_tag = ''
		sa_log_tag = ''
		sys_logger = None
		custom_handler = None

		@staticmethod
		def get_logger(moduleName):
			# If we have it already, return it directly
			if LogManager.log_handle == SYSLOG and platform.system() == 'Linux' and LogManager.sys_logger is not None:
				return logging.LoggerAdapter(LogManager.sys_logger, {'modulename': moduleName})

			if moduleName in LogManager.created_modules:
				return logging.getLogger(moduleName)
			logger = logging.getLogger(moduleName)
			logger.log_last_except = new.instancemethod(log_compact_traceback, logger, logger.__class__)
			logger.setLevel(LogManager.log_level)
			# add the handlers to logger
			logger.addHandler(LogManager._create_handler(logger))
			LogManager.created_modules.add(moduleName)

			if LogManager.log_handle == SYSLOG and platform.system() == 'Linux' and LogManager.sys_logger is not None:
				return logging.LoggerAdapter(LogManager.sys_logger, {'modulename': moduleName})

			return logger

		@staticmethod
		def _create_handler(logger):
			# create handler
			formatlist = ['%(asctime)s', LogManager.log_tag, '%(name)s', '%(levelname)s', '%(message)s']
			if LogManager.log_handle == SYSLOG:
				if platform.system() == 'Linux':
					# debug logs use LOG_LOCAL1
					ch = LH.SysLogHandler('/dev/log', facility=LH.SysLogHandler.LOG_LOCAL1)
					LogManager.sys_logger = logger
					formatlist = ['%(asctime)s', LogManager.log_tag, '%(modulename)s', '%(levelname)s', '%(message)s']
				else:
					ch = logging.FileHandler(LogManager.log_tag + "_" + time.strftime("%Y%m%d_%H%M%S") + '.log', encoding='utf8')
			elif LogManager.log_handle == FILE:
				ch = logging.FileHandler(LogManager.log_tag + "_" + time.strftime("%Y%m%d_%H%M%S") + '.log', encoding='utf8')
			elif LogManager.log_handle == CUSTOME:
				ch = LogManager.custom_handler()
			else:
				ch = logging.StreamHandler(sys.stdout)

			ch.setLevel(LogManager.log_level)
			# create formatter and add it to the handlers
			formatter = logging.Formatter(' - '.join(formatlist))
			ch.setFormatter(formatter)

			return ch

		@staticmethod
		def set_log_level(lv):
			LogManager.log_level = lv
			for name in LogManager.created_modules:
				logging.getLogger(name).setLevel(lv)

		@staticmethod
		def set_log_handle(handle):
			LogManager.log_handle = handle
			for name in LogManager.created_modules:
				logger = logging.getLogger(name)
				logger.handlers = []
				logger.addHandler(LogManager._create_handler(logger))

		@staticmethod
		def set_log_tag(log_tag):
			LogManager.log_tag = log_tag
			for name in LogManager.created_modules:
				logger = logging.getLogger(name)
				logger.handlers = []
				logger.addHandler(LogManager._create_handler(logger))

		@staticmethod
		def get_sa_logger():
			# If we have it already, return it directly
			if ('SALogger' in LogManager.created_modules):
				return SALogger(logging.getLogger('SALogger'))
			logger = logging.getLogger('SALogger')
			logger.setLevel(logging.INFO)
			if sys.platform.startswith('linux'):
				# linux下写到syslog
				ch = LH.SysLogHandler('/dev/log', facility=LH.SysLogHandler.LOG_LOCAL0)
				ch.setLevel(logging.INFO)
				formatter = logging.Formatter(LogManager.sa_log_tag + ": %(message)s")
				ch.setFormatter(formatter)
				logger.addHandler(ch)
			else:
				# windows，mac下写到文件
				filehandler = logging.FileHandler(LogManager.sa_log_tag + "_" + time.strftime("%Y%m%d_%H%M%S") + '.txt', encoding='utf8')
				formatter = logging.Formatter("%(message)s")
				filehandler.setFormatter(formatter)
				logger.addHandler(filehandler)
			LogManager.created_modules.add('SALogger')
			return SALogger(logger)

		@staticmethod
		def set_sa_log_tag(tag):
			"""设置运维日志tag"""
			LogManager.sa_log_tag = tag

		@staticmethod
		def set_custom_handler(handler):
			LogManager.log_handle = CUSTOME
			LogManager.custom_handler = handler

	class SALogger(object):
		def __init__(self, logger):
			self.logger = logger

		def log(self, operation, info_dict):
			log_time = time.strftime("%Y-%m-%d %H:%M:%S")
			json_str = json.dumps(info_dict, ensure_ascii=False)
			self.logger.info('[%s][%s],%s' % (log_time, operation, json_str))


def main():
	from common.mobilecommon import asiocore
	asiocore.start()
	logger = LogManager.get_logger("LogManager.Main")
	logger.debug("debug message")
	logger.info("info message")
	logger.warn("warn message")
	logger.error("error message")
	logger.critical("critical message")

	LogManager.set_sa_log_tag("H2_GameStatistic")
	sa_logger = LogManager.get_sa_logger()
	sa_logger.log('Install', {'install_time': 123456})

	try:
		raise StandardError("A")
	except:
		logger.log_last_except()

	while 1:
		asiocore.poll()

if __name__ == '__main__':
	main()
