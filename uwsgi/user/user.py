#coding: utf-8

from common import const

class User(object):

	def login(self, user_name, password):
		return const.SUCCESS, 'success'

	def register(self, user_info):
		pass

	def update_user(self, user_info):
		pass