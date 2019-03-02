#coding: utf-8

import os

from router import router
from common.log.log_manager import LogManager

LogManager.set_log_tag(str(os.getpid()))
_logger = LogManager.get_logger("mowang")

_router = router()

def info(*args):
	_logger.info(*args)

def get_router():
	return _router