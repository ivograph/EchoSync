// MyServer.cpp : Defines the entry point for the application.
//

#include "MyServer.h"

io_service service;

class talk_to_client : public boost::enable_shared_from_this<talk_to_client>
	, boost::noncopyable {
	typedef talk_to_client self_type;
	talk_to_client() : sock_(service), started_(false) {}
public:
	typedef boost::system::error_code error_code;
	typedef boost::shared_ptr<talk_to_client> ptr;

	void start() {
		started_ = true;
		do_login();
		//do_read();
		++number_clients_;
	}

	static ptr new_() {
		ptr new_(new talk_to_client);
		return new_;
	}
	void stop() {
		if (!started_) return;
		started_ = false;
		sock_.close();
		--number_clients_;

		std::cout << client_name_ << " left the room." << std::endl;
		if (number_clients_ == 1)
			std::cout << " There is " << number_clients_ << " person into the room." << std::endl;
		else
			std::cout << " There are " << number_clients_ << " people into the room." << std::endl;
	}
	ip::tcp::socket& sock() { return sock_; }

	void on_read(const error_code& err, size_t bytes) {
		if (!err) {
			std::string msg(read_buffer_, bytes);
			std::cout << client_name_ << ": " << msg;
			//do_write(msg + "\n");
			memset(read_buffer_, 0, sizeof(read_buffer_));
			do_read();
		}
		else
		{
			std::string error_test = err.message();
			stop();
		}
	}

	void on_login(const error_code& err, size_t bytes) {
		if (!err) {
			std::string msg(read_buffer_, bytes);

			char name[1024];
			sscanf_s(msg.data(), "login %s", name, (int)msg.size());
			client_name_ = name;

			std::cout << client_name_ << " enters into the room." << std::endl;
			if (number_clients_ == 1)
				std::cout << " There is " << number_clients_ << " person into the room." << std::endl;
			else
				std::cout << " There are " << number_clients_ << " people into the room." << std::endl;
			memset(read_buffer_, 0, sizeof(read_buffer_));
			do_read();
		}
		else
		{
			std::string error_test = err.message();
			stop();
		}
	}

	//void on_write(const error_code& err, size_t bytes) {
	//	do_read();
	//}

	void do_login()
	{
		if (sock_.is_open())
			async_read(sock_, buffer(read_buffer_), MEM_FN2(read_complete, _1, _2), MEM_FN2(on_login, _1, _2));
	}
	void do_read() {
		if(sock_.is_open())
			async_read(sock_, buffer(read_buffer_),	MEM_FN2(read_complete, _1, _2), MEM_FN2(on_read, _1, _2));
	}
	//void do_write(const std::string& msg) {
	//	if (!started()) return;
	//	std::copy(msg.begin(), msg.end(), write_buffer_);
	//	sock_.async_write_some(buffer(write_buffer_, msg.size()),	MEM_FN2(on_write, _1, _2));
	//}
	size_t read_complete(const boost::system::error_code& err, size_t	bytes) {
		if (err) return 0;
		bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
		// we read one-by-one until we get to enter, no buffering
		return found ? 0 : 1;
	}
	bool started() { return started_; }

private:
	ip::tcp::socket sock_;
	enum { max_msg = 1024 };
	char read_buffer_[max_msg];
	char write_buffer_[max_msg];
	std::string client_name_;
	bool started_;

	static int number_clients_;
};

int talk_to_client::number_clients_ = 0;

ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(),
  8001));

void handle_accept(talk_to_client::ptr client, const error_code& err)
{
  client->start();
  talk_to_client::ptr new_client = talk_to_client::new_();
  acceptor.async_accept(new_client->sock(),
    boost::bind(handle_accept, new_client, _1));
}


int main(int argc, char* argv[]) {
	std::cout << "The server is started."<<std::endl;
	talk_to_client::ptr client = talk_to_client::new_();
	acceptor.async_accept(client->sock(),
		boost::bind(handle_accept, client, _1));
	service.run();
}

