#include "iostream"
#include "iomanip"
#include "cstdlib"
#include <vector>
#include <map>

using namespace std;

int line[100][100];
int dx[8] = {-2, -1, 1,  2, -2, -1, 2, 1};
int dy[8] = {-1, -2, -2, -1, 1,  2, 1, 2};
int n, m;
int startx, starty;
int h1[6][6] = { 
	{ 1,30,33,16,3,24 }, { 32,17,2,23,34,15 }, { 29,36,31,14,25,4 }, 
	{ 18,9,6,35,22,13 }, { 7,28,11,20,5,26 }, { 10,19,8,27,12,21 }
};
int h2[6][8] = { 
	{ 1,10,31,40,21,14,29,38 },{ 32,41,2,11,30,39,22,13 },{ 9,48,33,20,15,12,37,28 },
	{ 42,3,44,47,6,25,18,23 },{ 45,8,5,34,19,16,27,36 },{ 4,43,46,7,26,35,24,17 } 
};
int h3[8][8] = { 
	{ 1,46,17,50,3,6,31,52 }, { 18,49,2,7,30,51,56,5 }, { 45,64,47,16,27,4,53,32 }, { 48,19,8,29,10,55,26,57 },
	{ 63,44,11,22,15,28,33,54 }, { 12,41,20,9,36,23,58,25 }, { 43,62,39,14,21,60,37,34 }, { 40,13,42,61,38,35,24,59 }
};
int h4[8][10] = { 
	{1,46,37,66,3,48,35,68,5,8}, { 38,65,2,47,36,67,4,7,34,69 }, { 45,80,39,24,49,18,31,52,9,6 }, { 64,23,44,21,30,15,50,19,70,33 },
	{ 79,40,25,14,17,20,53,32,51,10 }, { 26,63,22,43,54,29,16,73,58,71 }, { 41,78,61,28,13,76,59,56,11,74 }, { 62,27,42,77,60,55,12,75,72,57 }
};
int h5[10][10] = { 
	{ 1,54,69,66,3,56,39,64,5,8 }, { 68,71,2,55,38,65,4,7,88,63 }, { 53,100,67,70,57,26,35,40,9,6 }, { 72,75,52,27,42,37,58,87,62,89 },
	{ 99,30,73,44,25,34,41,36,59,10 }, { 74,51,76,31,28,43,86,81,90,61 }, { 77,98,29,24,45,80,33,60,11,92 }, { 50,23,48,79,32,85,82,91,14,17 },
	{ 97,78,21,84,95,46,19,16,93,12 }, { 22,49,96,47,20,83,94,13,18,15 }
};
int h6[10][12] = {
	{ 1,4,119,100,65,6,69,102,71,8,75,104 }, { 118,99,2,5,68,101,42,7,28,103,72,9 }, { 3,120,97,64,41,66,25,70,39,74,105,76 },
	{ 98,117,48,67,62,43,40,27,60,29,10,73 }, { 93,96,63,44,47,26,61,24,33,38,77,106 }, { 116,51,94,49,20,23,46,37,30,59,34,11 },
	{ 95,92,115,52,45,54,21,32,35,80,107,78 }, { 114,89,50,19,22,85,36,55,58,31,12,81 }, { 91,18,87,112,53,16,57,110,83,14,79,108 },
	{ 88,113,90,17,86,111,84,15,56,109,82,13 }
};

struct area_info {
	int row;
	int coln;
};

struct jump_info {
	int x;
	int y;
	area_info area;
};

std::map<int, jump_info> jump_map;

void split_area(int x, int y, int row, int coln, int total_coln)
{
	if (row <= 10 || coln <= 12) {
		return;
	}

	int area[4][2] = {};
	area[0][0] = int(row / 2);
	area[0][0] = area[0][0] - area[0][0] % 2;
	area[0][1] = int(coln / 2);
	area[0][1] = area[0][1] - area[0][1] % 2;
	split_area(x, y, area[0][0], area[0][1], total_coln);

	area[1][0] = area[0][0];
	area[1][1] = coln - area[0][1];
	split_area(x + area[0][1], y, area[1][0], area[1][1], total_coln);

	area[2][0] = row - area[0][0];
	area[2][1] = area[0][1];
	split_area(x, y + area[0][0], area[2][0], area[2][1], total_coln);

	area[3][0] = area[2][0];
	area[3][1] = area[1][1];
	split_area(x + area[0][1], y + area[0][0], area[2][0], area[2][1], total_coln);

	jump_info jump_0;
	jump_0.x = x + area[0][1] - 1;
	jump_0.y = y + area[0][0];
	jump_0.area.row = area[2][0];
	jump_0.area.coln = area[2][1];
	int index = (x + area[0][1] - 3) + (y + area[0][0] - 1) * total_coln;
	jump_map[index] = jump_0;

	jump_info jump_1;
	jump_1.x = x + area[0][1];
	jump_1.y = y + (area[0][0] + 1);
	jump_1.area.row = area[3][0];
	jump_1.area.coln = area[3][1];
	index = (x + area[0][1] - 2) + (y + area[0][0] + 2) * total_coln;
	jump_map[index] = jump_1;

	jump_info jump_2;
	jump_2.x = x + area[0][1];
	jump_2.y = (area[0][0] - 1);
	jump_2.area.row = area[1][0];
	jump_2.area.coln = area[1][1];
	index = x + area[0][1] + 2 + (y + area[0][0]) * total_coln;
	jump_map[index] = jump_2;

	jump_info jump_3;
	jump_3.x = x + area[0][1] - 1;
	jump_3.y = x + (area[0][0] - 2);
	jump_3.area.row = area[0][0];
	jump_3.area.coln = area[0][1];
	index = (x + area[0][1] + 1) + (y + area[0][0] - 3) * total_coln;
	jump_map[index] = jump_3;
}

int main()
{
	// https://blog.csdn.net/yuanyirui/article/details/3892583
	while(true) {
	cout << "input row£º";
	cin >> m;
	cout << "input coln£º";
	cin >> n;
	//memset(line, 0, sizeof(line));
	//cout << "start pos£º";
	//cin >> startx >> starty;
	//line[startx][starty] = 1;
	//int number = 1;
	//cout << "line£º\n";
	split_area(0, 0, m, n, n);
	for (auto itr = jump_map.begin(); itr != jump_map.end(); ++itr) {
		int index = itr->first;
		int x = int(index % n);
		int y = int(index / n);
		const jump_info& jump = itr->second;
		cout << "cur pos: " << x << " " << y << ", jump pos: " << jump.x << " " << jump.y << ", area: " << jump.area.row << " " << jump.area.coln << endl;
	}
	}
	return 0;
}