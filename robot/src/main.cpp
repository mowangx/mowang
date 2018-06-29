
#include "stdio.h"
#include <iostream>

#include "robot_server.h"

#include "binary_string.h"
#include "rpc_param.h"
#include "game_struct.h"

struct tests_struct
{
	uint16 a;
	char b;
	uint16 c;
};

template <class T>
void test(T& data) {
	std::cout << typeid(T).name() << std::endl;
}

int main(int argc, char* argv[])
{
	dynamic_string* bin_str = allocate_binary_string(sizeof(tests_struct));
	tests_struct in_s, out_s;
	int c;
	test(in_s);
	in_s.a = 0xdb;
	in_s.b = 'c';
	in_s.c = 0x87;
	int index = 0;
	flat_struct_2_bstr(bin_str->data(), in_s);
	bstr_2_flat_struct(out_s, bin_str->data());
	std::cout << "bin str" << bin_str->data() << std::endl;
	std::cout << "a is" << out_s.a << std::endl;
	std::cout << "b is" << out_s.b << std::endl;
	std::cout << "c is" << out_s.c << std::endl;


	char buffer[60000];
	int buffer_index = 0;
	fill_packet(buffer, buffer_index, (uint16)3);
	dynamic_array<soldier_info> soldiers;
	soldier_info soldier_1;
	soldier_1.soldier_type = 1;
	soldier_1.soldier_num = 100;
	soldiers.push_back(soldier_1);
	soldier_info soldier_2;
	soldier_2.soldier_type = 2;
	soldier_2.soldier_num = 200;
	soldiers.push_back(soldier_2);
	soldier_info soldier_3;
	soldier_3.soldier_type = 3;
	soldier_3.soldier_num = 300;
	soldiers.push_back(soldier_3);
	dynamic_string* bstr = allocate_binary_string(sizeof(soldier_info)* soldiers.size() + sizeof(uint16));
	dynamic_struct_2_bstr(bstr->data(), soldiers);

	//buffer_index = 0;
	//dynamic_array<soldier_info> out_soldiers;
	//db_param_parse<dynamic_array<soldier_info>, soldier_info>::parse_param(out_soldiers, bstr->data(), buffer_index);

	fill_packet(buffer, buffer_index, (TRoleID_t)0xF1F2F3F4F5F6F7F8, (TNpcIndex_t)0xA1A2,
		(TPosValue_t)0x1B2B, (TPosValue_t)0x1C2C, (TPosValue_t)0x1D2D, (TPosValue_t)0x1E2E, *bstr, (TGameTime_t)123456);
	fill_packet(buffer, buffer_index, (TRoleID_t)0xE1E2E3E4E5E6E7E8, (TNpcIndex_t)0xB2A3,
		(TPosValue_t)0x2B3B, (TPosValue_t)0x2C3C, (TPosValue_t)0x2D3D, (TPosValue_t)0x2E3E, *bstr, (TGameTime_t)234567);
	fill_packet(buffer, buffer_index, (TRoleID_t)0xD1D2D3D4D5D6D7D8, (TNpcIndex_t)0xA2A3,
		(TPosValue_t)0x2B3B, (TPosValue_t)0x2C3C, (TPosValue_t)0x2D3D, (TPosValue_t)0x2E3E, *bstr, (TGameTime_t)345678);
	dynamic_string result(buffer, buffer_index);

	buffer_index = 0;
	uint16 num = 0;
	rpc_param_parse<uint16, uint16>::parse_param(num, result.data(), buffer_index);
	for (int i = 0; i < num; ++i) {
		fight_info fight_data;
		rpc_param_parse<TRoleID_t, TRoleID_t>::parse_param(fight_data.role_id, result.data(), buffer_index);
		rpc_param_parse<TNpcIndex_t, TNpcIndex_t>::parse_param(fight_data.npc_id, result.data(), buffer_index);
		rpc_param_parse<TPosValue_t, TPosValue_t>::parse_param(fight_data.src_pos.x, result.data(), buffer_index);
		rpc_param_parse<TPosValue_t, TPosValue_t>::parse_param(fight_data.src_pos.y, result.data(), buffer_index);
		rpc_param_parse<TPosValue_t, TPosValue_t>::parse_param(fight_data.dest_pos.x, result.data(), buffer_index);
		rpc_param_parse<TPosValue_t, TPosValue_t>::parse_param(fight_data.dest_pos.y, result.data(), buffer_index);
		db_param_parse<dynamic_array<soldier_info>, soldier_info>::parse_param(fight_data.soldiers, result.data(), buffer_index);
		rpc_param_parse<TGameTime_t, TGameTime_t>::parse_param(fight_data.fight_time, result.data(), buffer_index);
		log_info("fight stub init data! role id = %" I64_FMT "u, src pox x = %d, dest pos x = %d, fight time = %u",
			fight_data.role_id, fight_data.src_pos.x, fight_data.dest_pos.x, fight_data.fight_time);
	}


	if (argc < 2) {
		std::cout << "argv is less than 2" << std::endl;
		return 0;
	}

	std::cout << "start robot" << argv[1] << std::endl;

	DRobotServer.start("robot", argv[1]);

	return 0;
}