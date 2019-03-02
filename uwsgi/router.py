#coding: utf-8

from common import const
from user.user import User

class router(object):

	def __init__(self):
		self.user = User()

	def parse_method(self, cls, method, params):
		obj = getattr(self, cls, None)
		if not obj:
			return const.ERROR_CLASS, ""

		func = getattr(obj, method, None)
		if not func:
			return const.ERROR_METHOD, ""

		argcount = func.im_func.func_code.co_argcount
		func_params = func.im_func.func_code.co_varnames[:argcount]

		args = []
		for param in func_params[1:]:
			if param not in params:
				return const.ERROR_PARAM, ""
			args.append(params[param])

		code, data = func(*args)
		return code, data
