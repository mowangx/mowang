# coding: utf-8

import sys
sys.path.append('lib')

import json
import gworld
import traceback

from common import utils
from common import const
from datetime import datetime

class http_request(object):

	def __init__(self, env):
		self.init(env)

	def init(self, env):
		self.path = env.get("PATH_INFO", None)
		self.query_string = env.get("QUERY_STRING", None)
		self.input = env.get("wsgi.input", None)
		self.content_len = int(env.get("CONTENT_LENGTH", -1))
		if self.content_len < 0 or self.content_len > 1024 * 1024:
			self.content_len = 1024 * 1024
		self.body = self.input.read(self.content_len)

	def __repr__(self):
		return "path %s, params %s" % (self.path, self.body)

class http_response(object):

	def __init__(self, start_response, code = 0, data = None):
		self.start_response = start_response
		self.headers = {}
		self.code = code
		self.data = data
		self.set_header("Content-Type", "text/html")

	def set_header(self, key, value):
		self.headers[key] = value

	def set_data(self, data):
		self.data = data

	def dumps(self):
		res_data = {
			"code": self.code,
			"msg": utils.get_error_msg(self.code),
			"result": self.data,
		}
		return json.dumps(res_data)

	def send(self):
		data = self.dumps()
		self.set_header("Content-Length", str(len(data)))
		headers = self.headers.items()
		self.start_response("200 OK", headers)
		return data

def process(request):
	paths = filter(lambda x: len(x) > 0, request.path.split("/"))
	if len(paths) != 2:
		return const.ERROR_PATH, ""

	try:
		cls, method = paths
		params = json.loads(request.body)
		params = utils.unicode_to_str(params)
		code, data = gworld.get_router().parse_method(cls, method, params)
		return code, data
	except Exception, e:
		traceback.print_exc()
		return const.ERROR_PARAM, ""

def application(environ, start_response):
	try:
		request = http_request(environ)
		recv_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S %f")
		gworld.info("recv time %s, request data is %s" % (recv_time, repr(request)))

		code, data = process(request)
		response = http_response(start_response, code, data)
		data = response.send()

		response_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S %f")
		gworld.info("response time %s, response data is %s" % (response_time, data))

	except Exception, e:
		data = traceback.format_exc()
		gworld.info(data)
		start_response("200 OK", [])
	return [data, ]
