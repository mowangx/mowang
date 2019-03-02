#coding: utf-8

import json

from http_request import http_request

params = {
	"user_name": "mowang",
	"password": "abc123"
}
rc, value = http_request("http://localhost:9010/user/login", body = json.dumps(params))
print rc, value