module CoreGlobal;

CoreGlobal::CoreGlobal()
{
	GThreadManager = new ThreadManager();
}

CoreGlobal::~CoreGlobal()
{
	delete GThreadManager;
}
