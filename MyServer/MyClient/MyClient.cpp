// MyServer.cpp : Defines the entry point for the application.
//

#include "MyClient.h"

io_service service;

const int max_body_length = 1024;

class talk_to_svr : public boost::enable_shared_from_this<talk_to_svr>
	, boost::noncopyable {
	typedef talk_to_svr self_type;
	talk_to_svr(const std::string& username)
		: sock_(service), started_(true), username_(username), timer_
		(service) {}
	void start(ip::tcp::endpoint ep) {
		sock_.async_connect(ep, MEM_FN1(on_connect, _1));
	}
public:
	typedef boost::system::error_code error_code;
	typedef boost::shared_ptr<talk_to_svr> ptr;
	static ptr start(ip::tcp::endpoint ep, const std::string&
		username) {
		ptr new_(new talk_to_svr(username));
		new_->start(ep);
		return new_;
	}
	void stop() {
		if (!started_) return;
		started_ = false;
		sock_.close();
	}
	bool started() { return started_; }
	
	void on_connect(const error_code& err) {
		if (!err)
		{
			do_write("login " + username_ + "\n");
		}
		else
		{
			std::cout << err.message();
			stop();
		}
	}
	//void on_read(const error_code& err, size_t bytes) {
	//	if (err) stop();
	//	if (!started()) return;
	//	// process the msg
	//	std::string msg(read_buffer_, bytes);
	//	if (msg.find("login ") == 0) on_login();
	//	else if (msg.find("ping") == 0) on_ping(msg);
	//	else if (msg.find("clients ") == 0) on_clients(msg);
	//}
	//void on_login() {
	//	do_ask_clients();
	//}
	//void on_ping(const std::string& msg) {
	//	std::istringstream in(msg);
	//	std::string answer;
	//	in >> answer >> answer;
	//	if (answer == "client_list_changed") do_ask_clients();
	//	else postpone_ping();
	//}
	//void on_clients(const std::string& msg) {
	//	std::string clients = msg.substr(8);
	//	std::cout << username_ << ", new client list:" << clients;
	//	postpone_ping();
	//}

	//void do_ping() { do_write("ping\n"); }
	//void postpone_ping() {
	//	timer_.expires_from_now(boost::posix_time::millisec(rand() %
	//		7000));
	//	timer_.async_wait(MEM_FN(do_ping));
	//}
	//void do_ask_clients() { do_write("ask_clients\n"); }
	void on_write(const error_code& err, size_t bytes) 
	{ 
		/*do_read();*/ 
		if (!!err)
		{
			std::string error_msg = err.message();
		}
	}
	//void do_read() {
	//	async_read(sock_, buffer(read_buffer_), MEM_FN2(read_complete, _1, _2), MEM_FN2(on_read, _1, _2));
	//}
	void do_write(const std::string& msg) {
		if (!started()) return;
		std::copy(msg.begin(), msg.end(), write_buffer_);
		sock_.async_write_some(buffer(write_buffer_, msg.size()), MEM_FN2(on_write, _1, _2));
	}
private:
	//size_t read_complete(const boost::system::error_code& err, size_t
	//	bytes) {
	//	if (err) return 0;
	//	bool found = std::find(read_buffer_, read_buffer_ + bytes,
	//		'\n')
	//		< read_buffer_ + bytes;
	//	return found ? 0 : 1;
	//}
private:
	ip::tcp::socket sock_;
	enum { max_msg = 1024 };
	char read_buffer_[max_msg];
	char write_buffer_[max_msg];
	bool started_;
	std::string username_;
	deadline_timer timer_;
};

#ifdef _my_threads
boost::thread_group threads;
void listen_thread() {
	service.run();
}
void start_listen(int thread_count) {
	for (int i = 0; i < thread_count; ++i)
		threads.create_thread(listen_thread);
}
#endif

void ClearConsole()
{
#if defined _WIN32
	system("cls");
	//clrscr(); // including header file : conio.h
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
	system("clear");
	//std::cout<< u8"\033[2J\033[1;1H"; //Using ANSI Escape Sequences 
#elif defined (__APPLE__)
	system("clear");
#endif
}

#define my_code
int main(int argc, char* argv[]) {
#ifdef my_code
	ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8001);
#ifdef _my_threads
	start_listen(1);
#endif
	std::cout << "user name: ";
	char consol_input[max_body_length + 1];
	std::cin.getline(consol_input, max_body_length + 1);
	auto talker = talk_to_svr::start(ep, consol_input);
	service.poll();

	ClearConsole();
	while(std::cin.getline(consol_input, max_body_length + 1) && strcmp(consol_input,"bye"))
	{
		talker->do_write(consol_input);
		boost::this_thread::sleep(boost::posix_time::millisec(100));
		service.poll();
	}
#ifdef _my_threads
	threads.join_all();
	service.run();
#endif
#else
	ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8001);
	const char* messages[] = { "John says hi", "so does James", "Lucy got home", 0 };
	for (const char** message = messages; *message; ++message)
	{
		talk_to_svr::start(ep, *message);
		boost::this_thread::sleep(boost::posix_time::millisec(100));
	}
	service.run();
#endif
}

