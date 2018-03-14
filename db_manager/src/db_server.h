
#ifndef _DB_SERVER_H_
#define _DB_SERVER_H_

class CDbConn;

class CDbServer
{
public:
	CDbServer();
	~CDbServer();

public:
	bool init();
	void run();

private:
	CDbConn * m_db;
};


#endif // !_DB_SERVER_H_

