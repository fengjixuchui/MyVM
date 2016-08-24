#include <iostream>
#include <fcntl.h>
#include <io.h>
#include <mvm.h>
using namespace std;
int main(int argc, char *argv[])
{
	cout << "�������ȡ�Ķ������ļ���";
	char *buf = new char[4096];
	cin >> buf;
	FILE *file = fopen(buf, "rb");
	byte *bs = new byte[filelength(fileno(file))+0xffff];
	read(fileno(file), bs, filelength(fileno(file)));

    VMState state;
	state.pausecbk = []() {cout << "pause"; };
	state.pointcbk = [](qword id) {cout << "point"<<id; };
	state.len = 88;
	state.programptr = (void *)bs;
	auto a=new Array<qword, dword>(10);//10���Ĵ���
	state.regs.movein(*a);
	state.regs.data[0] = 0;//�����0��ʼִ��
    MVM *vm=new MVM(state);
    cout<<vm->havestop();
    vm->start();
    cout<<vm->havestop();
    vm->stop();
    cout<<vm->havestop();
    cin.get();
    return 0;
}
