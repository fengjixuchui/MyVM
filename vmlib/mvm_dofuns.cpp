#include "mvm.h"
///ע������Ϊִ�к���ϵ�У���
#include <iostream>
#pragma region ָ��ִ�к���
void MVM::doone()
{
	//ע�� �������ֻ�����ڲ�״̬
	//��ʼǰ�ֱ���isrupt��� �� nowstep��ǣ����ڲ�״̬�У�
	//��ѭ���жϺ�ͣ����ԭ��
	if (isrupt)
	{
		if (!isinrupt)
		{
			//��������ж��оͿ�ʼ�ж�
			rupt(ruptid);
			isrupt = false;
			return;//��ʼ�ж�ִ�� ���ָ�ִ��ֱ�ӷ���
		}
		else isrupt = false;//�����Ƿ���Ӧ���ж϶�ֱ�����ô˱�־ ��������ж����������ж�����
		////�Ժ���ܻ���Ϲ����ж�����ģʽ��ֱ�ӺͶ���ʽ���Ĺ���
	}
	if (!instate.points.isnull()&&instate.nowstep == instate.points[instate.curpoint])
	{
		//���öϵ�ص�����
		lockdo(outstate = instate);
		instate.pointcbk(instate.curpoint); //���ﲻ����û�����öϵ�ص���������� Ҳ����˵��������
		//����������ж�����������������߳�
	}
	//���´���ָ��
	//��ȡָ��
	//��ȡִ��ָ��Ĵ���������
	qword &pptr = instate.regs[0];
	if (pptr >= instate.len) throw "���󣬳���ִ��Խ�磡";//���ִ�г���������ڴ�ռ��С���׳��쳣
	byte *coms =((byte *)instate.programptr)+pptr;//�õ���ǰָ���ָ��

	if (getmptr) getmemptr(*(qword *)coms);//�����ȡ�ڴ��ַ��Ǿ�ֱ��ȡ��ַ

#define getid(type,pt) *(type *)(coms+pt)
 	//��ȡָ���� Ҳ���ǵ�һ���ֽ�
	byte com = coms[0];
	switch (com)
	{
	case 0:
		//�ڲ��ж�ָ��
		//�ȱ�ʾ��ָ���Ѿ���������
		pptr+=8;
		if (isinrupt) break;//��������жϾ�ֱ�Ӳ�����
		//���������ж�
		//��ȡ�ж���
		{
			//����case���ܶ������������һ������
			dword id = getid(dword,4);//�õ��ж�id
			rupt(id);//�����жϹ���
		};
		break;
	case 1:
		pptr+=8;
		{
			dword regid = getid(dword,2);//��üĴ���id
			word sid = getid(word,6);//����ݴ���id
			leareg(regid, sid);
		};
		break;
	case 2:
		pptr+=8;
		{
			word id = getid(word, 6);
			leamem(id);
		};
		break;
	case 3:
		pptr+=8;
		{
			word id = getid(word, 6);
			ld(id);
		};
		break;
	case 4:
		pptr+=8;
		{
			dword id = getid(dword, 4);
			ldl(id);
		};
		break;
	case 5:
		pptr+=8;
		{
			dword id = getid(dword, 4);
			ldh(id);
		};
		break;
	case 6:
		pptr+=8;
		{
			word id = getid(word, 6);
			save(id);
		};
		break;
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
		pptr+=8;
		{
			word id = getid(word, 6);
			decltype(&MVM::add) fun;
			switch (com)
			{
			case 7:
				fun = &MVM::add;
				break;
			case 8:
				fun = &MVM::dec;
				break;
			case 9:
				fun = &MVM::mul;
				break;
			case 10:
				fun =&MVM::div;
				break;
			case 11:
				fun = &MVM::cmp;
				break;
			}
			//���ﲻ�����������
			(this->*fun)(id);//ֱ�ӵ���
		};
		break;
	case 12:
		pptr+=8;
		pause();
		break;
	case 13:
		pptr+=8;
		step();
		break;
	case 14:
		pptr+=8;
		jmp();
		break;
	case 15:
		pptr+=8;
		{
			byte id = getid(byte, 7);
			asjmp(id);
		}
		break;
	case 16:
		pptr+=8;
		push();
		break;
	case 17:
		pptr+=8;
		pop();
		break;
	case 18:
		pptr+=8;
		{
			dword id = getid(dword, 4);
			read(id);
		}
		break;
	case 19:
		pptr+=8;
		{
			dword id = getid(dword, 4);
			write(id);
		}
		break;
	case 20:
		pptr+=8;
		call();
		break;
	case 21:
		pptr+=8;
		ret();
		break;
	case 22:
		pptr+=8;
		iret();
		break;
	default:
		pptr+=8;
		//ָ����� ����ָ������ж� �ⲿ�ж�0��
		startrupt(0);
		break;
	}
	//�ڴ˲����κ����� 
	//��׼�趨Ϊָ��ִ���겻�����κζ�������
	//�κζ����¼�������һ��ָ�ʼǰִ��
}
#pragma endregion

//����Ĵ������� 0 ����ִ�мĴ��� ���浱ǰִ��λ��ָ��(��Գ�����ʼ��ַ)
// 1 �ж�������ָ��Ĵ���  2 �ж��������ȼĴ��� 3 ջָ��Ĵ��� 4 ��־�Ĵ��� 
//��־�Ĵ��������λΪ�Ƚ�λ ��������λ�������ж���Ӧλ Ϊ��ʱ�����ж�Ҳ�����ж�������

//�ڲ��ж�ָ�����
void MVM::rupt(dword id)
{
	auto a = (instate.regs[4] & (((qword)1) << 2));
	if ((instate.regs[4] & (((qword)1) << 2)) == 0)
	{
		//���������ж�ָ��
		switch (id)
		{
		case 0:
			std::cerr << "ָ�����";
			return;
		case 1:
			std::cerr << "��0����";
			return;
		case 2:
			std::cerr << "�˿ڲ�����";
			return;
		case 3:
			std::cerr << "�Ĵ���������";
			return;
		case 4:
			std::cerr << "�жϺų���";
			return;
		}
	}
	//���ж�������õ���ַ
	qword tabp = instate.regs[1];
	qword len = instate.regs[2];
	if (id >= len) 
	{
		startrupt(4); return;
	}//�����ų���
	//����ʼ�ж�
	qword data = ((qword *)tabp)[id];//�õ��жϺ�����ַ
	isinrupt = true;//���ñ�־
	//��ת
	tocall(data);
	
}

void MVM::leareg(dword regid, word id)
{
	if (regid >= instate.regs.sum) { startrupt(3); return; }
	qword *ptr = instate.regs.at(regid);
	ptrs[id] = ptr;
	//�ݴ���Խ�粻���� ��Ϊ��65536��
}

void MVM::leamem(word id)
{
	getmpi = id;
	getmptr = true;
}


void MVM::ld(word id)
{
	currdata = *ptrs[id];
}

void MVM::ldl(dword data)
{
	*(dword*)(&currdata) = data;
}

void MVM::ldh(dword data)
{
	*(dword*)(&currdata+4) = data;
}

void MVM::save(word id)
{
	*ptrs[id] = currdata;
}

void MVM::add(word id)
{
	*ptrs[id] += currdata;
}

void MVM::dec(word id)
{
	*ptrs[id] -= currdata;
}

void MVM::mul(word id)
{
	*ptrs[id] *= currdata;
}

void MVM::div(word id)
{
	*ptrs[id] /= currdata;
}
//index��0��ʼ
#define settrue(obj,index) obj=obj|((qword)1<<index)
#define setfalse(obj,index) obj=obj&(~((qword)1<<index))
void MVM::cmp(word id)
{
	//������бȽ�
	if (currdata > *ptrs[id])
	{
		settrue(instate.regs[4], 1);
		setfalse(instate.regs[4], 0);
		//10�������
	}
	else if (currdata == *ptrs[id])
	{
		settrue(instate.regs[4], 0);
		setfalse(instate.regs[4], 1);
	}
	else
	{
		setfalse(instate.regs[4], 0);
		setfalse(instate.regs[4], 1);
	}
	//������Ϊ������������
}

void MVM::pause()
{
	//���Ƴ�
	lockdo(outstate = instate);
	instate.pausecbk();//������ͣ����
}

void MVM::step()
{
	instate.nowstep++;
}

void MVM::jmp()
{
	tojmp(currdata);
}

void MVM::asjmp(byte d)
{
	//�Ƚ������λ ����ֻ��һ������û��д�ɺ�
	if ((d & 0b00000011) == ((byte)instate.regs[4]) & 0b00000011)
	{
		jmp();
	}
}

void MVM::push()
{
	qword *stackptr_ptr = instate.regs.at(3);
	qword *&stackptr = *(qword **)stackptr_ptr;
	stackptr--;//�Ӹߵ��� �ȸı�ָ������ջ
	*stackptr = currdata;

}

void MVM::pop()
{
	qword *stackptr_ptr = instate.regs.at(3);
	qword *&stackptr = *(qword **)stackptr_ptr;
	currdata=*stackptr;
	stackptr++;//��ȡջ���ټ�ջָ�� �ӵ͵���
}

void MVM::read(dword id)
{
	//��ö˿ڵ����ݵ�ַ
	auto ptr = instate.ports.find(id);
	if (ptr == instate.ports.end()) { startrupt(2); return; }
	IOPort port = ptr->second;
	dword data = *port.portdata;
	//���������ݴ���
	currdata = data;//�ŵ��ݴ����ĵ�λ����
}

void MVM::write(dword id)
{
	//����ճ���Ĵ��� ��ʵ���԰������һ���ִ����װ��һ������
	//��ö˿����ú���
	auto ptr = instate.ports.find(id);
	if (ptr == instate.ports.end()) { startrupt(2); return; }
	IOPort port = ptr->second;
	auto fun = port.portset;

	//�������� ע��˴����衰Ӳ�����ӿ���Ӧ���������ȡ���޸��ⲿ״̬ Ҳ���ǲ�����������ļ��ӻ
	fun(currdata);
}

void MVM::call()
{
	tocall(currdata);
}

void MVM::ret()
{
	auto temp = currdata;
	pop();//�õ����ص�ַ
	tojmp(currdata);//��ת����
	currdata = temp;//��ԭ����
}

void MVM::iret()
{
	isinrupt = false;
	ret();
}

void MVM::tojmp(qword ptr)
{
	instate.regs[0] = ptr;//��ת
}

void MVM::tocall(qword ptr)
{
	qword reg0 = instate.regs[0];
	auto temp = currdata;
	currdata = reg0;
	push();
	currdata = temp;
	tojmp(ptr);//�ȱ������ת
}

void MVM::getmemptr(qword ptr)
{
	qword *p = (qword*)((byte *)instate.programptr) + ptr;
	ptrs[getmpi]=p;
}
