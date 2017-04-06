int foo(int b){
	return b;
}
int main(int argc, char ** argv){
	int a = 3;
	int b = foo(argc > 5? a : -42);
	return b;
}
