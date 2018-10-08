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
bool first_flag = true;
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
	int absolute_x;
	int absolute_y;
	area_info area;
};
jump_info start_area;
int ary[100][100];

std::map<int, jump_info> jump_map;
std::map<int, int> jump_pair;
std::vector<int> jump_ary;

void split_area(int x, int y, int row, int coln, int total_coln)
{
	if ((row + coln) <= 22) {
		//cout << "start update area, pos: " << x << " " << y << endl;
		for (int i = 0; i < row; ++i) {
			for (int j = 0; j < coln; ++j) {
				if (startx == (x + j) && starty == (y + i)) {
					start_area.area.row = row;
					start_area.area.coln = coln;
					start_area.x = startx;
					start_area.y = starty;
					start_area.absolute_x = j;
					start_area.absolute_y = i;
				}
				int idx = x + j + (y + i) * total_coln;
				std::map<int, int>::iterator pair_itr = jump_pair.find(idx);
				if (pair_itr == jump_pair.end()) {
					continue;
				}
				//cout << "update area, pos: " << x + j << " " << y + i << endl;
				std::map<int, jump_info>::iterator itr = jump_map.find(pair_itr->second);
				if (itr == jump_map.end()) {
					cout << "not find" << x + j << "  " << y + i << endl;
					continue;
				}
				jump_info& jump = itr->second;
				jump.area.row = row;
				jump.area.coln = coln;
			}
		}
		return;
	}

	int area[4][2] = {};
	area[0][0] = int(row / 2);
	area[0][0] = area[0][0] - area[0][0] % 2;
	area[0][1] = int(coln / 2);
	area[0][1] = area[0][1] - area[0][1] % 2;

	area[1][0] = area[0][0];
	area[1][1] = coln - area[0][1];

	area[2][0] = row - area[0][0];
	area[2][1] = area[0][1];

	area[3][0] = area[2][0];
	area[3][1] = area[1][1];

	jump_info jump_0;
	jump_0.x = x + area[0][1] - 1;
	jump_0.y = y + area[0][0];
	jump_0.absolute_x = -1;
	jump_0.absolute_y = 0;
	int index = (x + area[0][1] - 3) + (y + area[0][0] - 1) * total_coln;
	jump_map[index] = jump_0;
	int pair_index = jump_0.x + jump_0.y * total_coln;
	jump_pair[pair_index] = index;

	jump_info jump_1;
	jump_1.x = x + area[0][1];
	jump_1.y = y + (area[0][0] + 1);
	jump_1.absolute_x = 0;
	jump_1.absolute_y = 1;
	index = (x + area[0][1] - 2) + (y + area[0][0] + 2) * total_coln;
	jump_map[index] = jump_1;
	pair_index = jump_1.x + jump_1.y * total_coln;
	jump_pair[pair_index] = index;

	jump_info jump_2;
	jump_2.x = x + area[0][1];
	jump_2.y = y + (area[0][0] - 1);
	jump_2.absolute_x = 0;
	jump_2.absolute_y = -1;
	index = (x + area[0][1] + 2) + (y + area[0][0]) * total_coln;
	jump_map[index] = jump_2;
	pair_index = jump_2.x + jump_2.y * total_coln;
	jump_pair[pair_index] = index;

	jump_info jump_3;
	jump_3.x = x + area[0][1] - 1;
	jump_3.y = y + (area[0][0] - 2);
	jump_3.absolute_x = -1;
	jump_3.absolute_y = -2;
	index = (x + area[0][1] + 1) + (y + area[0][0] - 3) * total_coln;
	jump_map[index] = jump_3;
	pair_index = jump_3.x + jump_3.y * total_coln;
	jump_pair[pair_index] = index;

	cout << "split area, 1 row is: " << row << ", coln is: " << coln << " " << area[0][0] << " " << area[0][1] << endl;
	cout << "split area, 2 row is: " << row << ", coln is: " << coln << " " << area[1][0] << " " << area[1][1] << endl;
	cout << "split area, 3 row is: " << row << ", coln is: " << coln << " " << area[2][0] << " " << area[2][1] << endl;
	cout << "split area, 4 row is: " << row << ", coln is: " << coln << " " << area[3][0] << " " << area[3][1] << endl;

	split_area(x, y, area[0][0], area[0][1], total_coln);
	split_area(x + area[0][1], y, area[1][0], area[1][1], total_coln);
	split_area(x, y + area[0][0], area[2][0], area[2][1], total_coln);
	split_area(x + area[0][1], y + area[0][0], area[3][0], area[3][1], total_coln);
}

void show(const jump_info& jump, int& sequence, int coln);

bool need_jump(int x, int y, int coln) {
	int index = x + y * coln;
	std::map<int, jump_info>::iterator itr = jump_map.find(index);
	return itr != jump_map.end();
}

void check_jump(int x, int y, int coln, int sequence) {
	int index = x + y * coln;
	std::map<int, jump_info>::iterator itr = jump_map.find(index);
	if (itr != jump_map.end()) {
		if (std::find(jump_ary.begin(), jump_ary.end(), index) == jump_ary.end()) {
			jump_ary.push_back(index);
		}
		else {
			return;
		}
		const jump_info& jump = itr->second;
		cout << "start jump: " << jump.x << " " << jump.y << endl;
		if (jump.x == startx && jump.y == starty) {
			cout << "end jump" << endl;
			return;
		}
		first_flag = false;
		show(itr->second, sequence, coln);
	}
}

void calc_absolute_pos(int& x, int& y, const jump_info& jump)
{
	x = jump.absolute_x;
	if (x < 0) {
		x += jump.area.coln;
	}
	y = jump.absolute_y;
	if (y < 0) {
		y += jump.area.row;
	}
}

void show_detail_1(const jump_info& jump, int& sequence, int coln, bool order_flag = true)
{
	int num = 0;
	int absolute_x(0), absolute_y(0);
	calc_absolute_pos(absolute_x, absolute_y, jump);
	int absolute_value = h1[absolute_y][absolute_x];
	for (int i = 0; i < 36; ++i) {
		for (int k = 0; k < 6; ++k) {
			for (int l = 0; l < 6; ++l) {
				int x = jump.x - absolute_x + k;
				int y = jump.y - absolute_y + l;
				int cur_value = 0;
				if (order_flag) {
					cur_value = (36 + h1[l][k] - absolute_value) % 36;
				}
				else {
					cur_value = (36 - h1[l][k] + absolute_value) % 36;
				}
				if (cur_value == i) {
					++num;
					if (!first_flag && need_jump(x, y, coln) && num < 3) {
						sequence -= 1;
						show_detail_1(jump, sequence, coln, false);
						return;
					}
					++sequence;
					ary[y][x] = sequence;
					check_jump(x, y, coln, sequence);
				}
			}
		}
	}
}

void show_detail_2(const jump_info& jump, int& sequence, int coln, bool reverse_flag)
{
	int absolute_x(0), absolute_y(0);
	calc_absolute_pos(absolute_x, absolute_y, jump);
	int absolute_value = h2[absolute_y][absolute_x];
	for (int i = 0; i < 48; ++i) {
		for (int k = 0; k < 6; ++k) {
			for (int l = 0; l < 8; ++l) {
				if (reverse_flag) {
					int cur_value = (48 + h2[k][l] - absolute_value) % 48;
					if (cur_value == i) {
						int x = jump.x + k - absolute_y;
						int y = jump.y + l - absolute_x;
						++sequence;
						ary[y][x] = sequence;
						check_jump(x, y, coln, sequence);
					}
				}
				else {
					int cur_value = (48 + h2[l][k] - absolute_value) % 48;
					if (cur_value == i) {
						int x = jump.x + l - absolute_y;
						int y = jump.y + k - absolute_y;
						++sequence;
						ary[y][x] = sequence;
						check_jump(x, y, coln, sequence);
					}
				}
			}
		}
	}
}

void show_detail_3(const jump_info& jump, int& sequence, int coln)
{
	int absolute_x(0), absolute_y(0);
	calc_absolute_pos(absolute_x, absolute_y, jump);
	int absolute_value = h3[absolute_y][absolute_x];
	for (int i = 0; i < 64; ++i) {
		for (int k = 0; k < 8; ++k) {
			for (int l = 0; l < 8; ++l) {
				int x = jump.x + k - absolute_y;
				int y = jump.y + l - absolute_x;
				int cur_value = (64 + h3[k][l] - absolute_value) % 64;
				if (cur_value == i) {
					++sequence;
					ary[y][x] = sequence;
					check_jump(x, y, coln, sequence);
				}
			}
		}
	}
}

void show_detail_4(const jump_info& jump, int& sequence, int coln, bool reverse_flag)
{
	int absolute_x(0), absolute_y(0);
	calc_absolute_pos(absolute_x, absolute_y, jump);
	int absolute_value = h4[absolute_y][absolute_x];
	for (int i = 0; i < 80; ++i) {
		for (int k = 0; k < 8; ++k) {
			for (int l = 0; l < 10; ++l) {
				if (reverse_flag) {
					int cur_value = (80 + h4[k][l] - absolute_value) % 80;
					if (cur_value == i) {
						int x = jump.x + k - absolute_y;
						int y = jump.y + l - absolute_x;
						++sequence;
						ary[y][x] = sequence;
						check_jump(x, y, coln, sequence);
					}
				}
				else {
					int cur_value = (80 + h4[l][k] - absolute_value) % 80;
					if (cur_value == i) {
						int x = jump.x + l - absolute_y;
						int y = jump.y + k - absolute_x;
						++sequence;
						ary[y][x] = sequence;
						check_jump(x, y, coln, sequence);
					}
				}
			}
		}
	}
}

void show_detail_5(const jump_info& jump, int& sequence, int coln)
{
	int absolute_x(0), absolute_y(0);
	calc_absolute_pos(absolute_x, absolute_y, jump);
	int absolute_value = h5[absolute_y][absolute_x];
	for (int i = 0; i < 100; ++i) {
		for (int k = 0; k < 10; ++k) {
			for (int l = 0; l < 10; ++l) {
				int x = jump.x + k - absolute_y;
				int y = jump.y + l - absolute_x;
				int cur_value = (100 + h5[k][l] - absolute_value) % 100;
				if (cur_value == i) {
					++sequence;
					ary[y][x] = sequence;
					check_jump(x, y, coln, sequence);
				}
			}
		}
	}
}

void show_detail_6(const jump_info& jump, int& sequence, int coln, bool reverse_flag)
{
	int absolute_x(0), absolute_y(0);
	calc_absolute_pos(absolute_x, absolute_y, jump);
	int absolute_value = h6[absolute_y][absolute_x];
	for (int i = 0; i < 120; ++i) {
		for (int k = 0; k < 10; ++k) {
			for (int l = 0; l < 12; ++l) {
				if (reverse_flag) {
					int cur_value = (120 + h6[k][l] - absolute_value) % 120;
					if (cur_value  == i) {
						int x = jump.x + k - absolute_y;
						int y = jump.y + l - absolute_x;
						++sequence;
						ary[y][x] = sequence;
						check_jump(x, y, coln, sequence);
					}
				}
				else {
					int cur_value = (120 + h6[l][k] - absolute_value) % 120;
					if (cur_value == i) {
						int x = jump.x + l - absolute_y;
						int y = jump.y + k - absolute_x;
						++sequence;
						ary[y][x] = sequence;
						check_jump(x, y, coln, sequence);
					}
				}
			}
		}
	}
}

void show(const jump_info& jump, int& sequence, int coln)
{
	if (jump.area.row == 6 && jump.area.coln == 6) {
		show_detail_1(jump, sequence, coln);
	}
	else if (jump.area.row == 6 && jump.area.coln == 8) {
		show_detail_2(jump, sequence, coln, false);
	}
	else if (jump.area.row == 8 && jump.area.coln == 6) {
		show_detail_2(jump, sequence, coln, true);
	}
	else if (jump.area.row == 8 && jump.area.coln == 8) {
		show_detail_3(jump, sequence, coln);
	}
	else if (jump.area.row == 8 && jump.area.coln == 10) {
		show_detail_4(jump, sequence, coln, false);
	}
	else if (jump.area.row == 10 && jump.area.coln == 8) {
		show_detail_4(jump, sequence, coln, true);
	}
	else if (jump.area.row == 10 && jump.area.coln == 10) {
		show_detail_5(jump, sequence, coln);
	}
	else if (jump.area.row == 10 && jump.area.coln == 12) {
		show_detail_6(jump, sequence, coln, false);
	}
	else if (jump.area.row == 12 && jump.area.coln == 10) {
		show_detail_6(jump, sequence, coln, true);
	}
}

int main()
{
	// https://blog.csdn.net/yuanyirui/article/details/3892583
	while(true) {
	cout << "input row£º";
	cin >> m;
	cout << "input coln£º";
	cin >> n;
	memset(ary, 0, sizeof(ary));
	//memset(line, 0, sizeof(line));
	cout << "start pos£º";
	cin >> startx >> starty;
	//line[startx][starty] = 1;
	//int number = 1;
	//cout << "line£º\n";
	jump_map.clear();
	jump_pair.clear();
	split_area(0, 0, m, n, n);
	for (auto itr = jump_map.begin(); itr != jump_map.end(); ++itr) {
		int index = itr->first;
		int x = int(index % n);
		int y = int(index / n);
		const jump_info& jump = itr->second;
		cout << "cur pos: " << x << " " << y << ", jump pos: " << jump.x << " " << jump.y << ", area: " << jump.area.row << " " << jump.area.coln << endl;
		int idx = jump.x + jump.y * n;
		auto pair_itr = jump_pair.find(idx);
		if (pair_itr == jump_pair.end()) {
			cout << "cur pos not find" << x << " " << y << endl;
		}
		else {
			index = pair_itr->second;
			x = index % n;
			y = index / n;
			//cout << "pair pos: " << x << " " << y << endl;;
		}
	}
	int sequence = 0;
	show(start_area, sequence, n);
	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n; ++j) {
			cout << ary[i][j] << "  ";
		}
		cout << endl;
	}
	}
	return 0;
}