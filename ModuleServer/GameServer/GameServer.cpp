import Pch;
import RefCounting;
import Memory;

using namespace std;

class Person
{
public:
	Person() { }
	virtual ~Person() { }

	virtual void hp() const
	{
		cout << 300 << "\n";
	}

	void Inorder()
	{
		cout << 410 << "\n";
	}
};


class Knight : public Person
{
public:
};

int main()
{
	Knight* knight = xnew<Knight>();

	knight->hp();
	knight->Inorder();

	xdelete(knight);
}