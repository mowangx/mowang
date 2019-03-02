#coding: utf-8

import gzip
import pycurl
import cStringIO as StringIO


def get_response_headers(headerstr):
	headers = {}
	lines = headerstr.split("\r\n")
	for i, line in enumerate(lines):
		if i == 0 or len(line.strip()) == 0:
			continue
		k, v = map(lambda x: x.strip(), line.strip().split(":", 1))
		headers[k] = v
	return headers


def http_request(url, headers={}, body=None, connect_timeout=10, timeout=60):
	try:
		c = pycurl.Curl()
		str = StringIO.StringIO()
		rheader = StringIO.StringIO()
		c.setopt(pycurl.URL, url)
		c.setopt(pycurl.CONNECTTIMEOUT, connect_timeout)
		c.setopt(pycurl.TIMEOUT, timeout)  # 下载超时
		c.setopt(pycurl.WRITEFUNCTION, str.write)
		c.setopt(pycurl.FOLLOWLOCATION, 1)
		# c.setopt(pycurl.HEADER, True)

		H = []
		for k, v in headers.iteritems():
			H.append("%s: %s" % (k, v))

		c.setopt(pycurl.HTTPHEADER, H)

		if body is not None:
			c.setopt(pycurl.POSTFIELDS, body)

		c.setopt(pycurl.SSL_VERIFYPEER, 0)
		c.setopt(pycurl.SSL_VERIFYHOST, 0)
		c.setopt(c.HEADERFUNCTION, rheader.write)

		c.perform()
		code = c.getinfo(pycurl.RESPONSE_CODE)
		if code != 200:
			# print "ERROR response code is %s"% (code)
			return False, "code error %s" % (code)

		rheaders = rheader.getvalue()
		rheaders = get_response_headers(rheaders)

		if rheaders.get('Content-Encoding', None) == "gzip":
			str.seek(0)
			gzfile = gzip.GzipFile(fileobj=str)
			value = gzfile.read()
			gzfile.close()
			str.close()
		else:
			value = str.getvalue()
			str.close()
	except pycurl.error, e:
		return False, (e.args, e.message)

	except Exception, e:
		return False, (e.args, e.message)

	return True, value
