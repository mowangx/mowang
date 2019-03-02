#coding: utf-8

import const

def unicode_to_str(d):
	if type(d) is dict:
		output = {}
		for k, v in d.iteritems():
			output[unicode_to_str(k)] = unicode_to_str(v)
		return output
	elif type(d) is list or type(d) is tuple:
		output = [unicode_to_str(v) for v in d]
		return output
	elif type(d) is unicode:
		return d.encode('utf-8')
	else:
		return d

def get_error_msg(err_code):
	err_code_2_msg = {
		const.SUCCESS: "success",
		const.ERROR_PATH: "path error",
		const.ERROR_PARAM: "param error",
	}
	return err_code_2_msg.get(err_code, "failed")