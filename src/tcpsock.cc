#include <tcpsock.h>

namespace parse {
  auto request(std::istringstream req) {
  	std::vector<std::string> p;

    std::string token;
  	while (req >> token) {
  	  p.push_back(token); 
  	}
  	return p;
  }
}

namespace serv {
  int svfd, clfd;
  int opt = 1;
  struct sockaddr_in addr = { AF_INET, htons(80), INADDR_ANY };
  std::string static_file;

  namespace controller {
    void redirect(std::string uri) {
      std::string header = 
        "HTTP/1.1 301 Moved Permanently\r\n"
        "Location: " + uri + "\r\n"
        "\n";
      write(clfd, &header[0], header.size()); // send header
    }

    void load(std::vector<std::string> req) {
      ssize_t find_version = req[1].find("?");
      if (find_version > 0) {
        req[1] = req[1].substr(0, find_version);
      }

      if (req[1] == "/" || (ssize_t) req[1].find(".html") > 0) {
        return;
      }

      std::ifstream f(static_file + req[1]);

      if (!f) {
        return redirect("/404.html");
      }

      std::string header = 
        "HTTP/1.1 200 OK\r\n"
        "\n";
      write(clfd, &header[0], header.size()); // send header

      std::string buf(256, 0);
      while (f) {
        f.read(&buf[0], buf.size());
        write(clfd, &buf[0], buf.size()); // send body
      }

      f.close(); 
    }

    void redirect_index(std::vector<std::string> req) {
      redirect("/index.html");
    }

    void redirect_info(std::vector<std::string> req) {
      redirect("/info.html");
    }

    void redirect_404(std::vector<std::string> req) {
      redirect("/404.html");
    }

    void render_index(std::vector<std::string> req) {
      std::ifstream f(static_file + "/index.html");

      if (!f) {
        return redirect("/404.html");
      }

      std::string header = 
        "HTTP/1.1 200 OK\r\n"
        "\n";
      write(clfd, &header[0], header.size()); // send header

      std::string buf;
      while (f) {
        getline(f, buf);
        write(clfd, &buf[0], buf.size()); // send body
      }

      f.close();
    }

    void render_info(std::vector<std::string> req) {
      std::ifstream f(static_file + "/info.html");

      if (!f) {
        return redirect("/404.html");
      }

      std::string header = 
        "HTTP/1.1 200 OK\r\n"
        "\n";
      write(clfd, &header[0], header.size()); // send header
      
      std::string buf;
      while (getline(f, buf)) {
        write(clfd, &buf[0], buf.size()); // send body
      }
      f.close();
    }

    void render_404(std::vector<std::string> req) {
      std::ifstream f(static_file + "/404.html");

      if (!f) {
        return redirect("/404.html");
      }

      std::string header = 
        "HTTP/1.1 404 Not Found\r\n"
        "\n";
      write(clfd, &header[0], header.size()); // send header

      std::string buf;
      while (f) {
        getline(f, buf);
        write(clfd, &buf[0], buf.size()); // send body
      }

      f.close();
    }

    void auth(std::vector<std::string> req) {
      for (auto i : req) {
        if (i == "username=admin&password=admin") {
          return redirect("/info.html");
        }
      }
      return redirect("/index.html");
    }
  }

  namespace route {
    int get(std::string uri, std::vector<std::string> req, void (*f)(auto)) {
      if (req[0] == "GET" && (req[1] == uri || "*" == uri)) {
    	  f(req);
    	  return 1;
    	}
  	  return 0;
    }

    int post(std::string uri, std::vector<std::string> req, void (*f)(auto)) {
      if (req[0] == "POST" && req[1] == uri) {
    	  f(req);
    	  return 1;
    	}
    	return 0;
    }
  }

  void conn() {
    // read request
    std::string data;
    std::string buf(256, 0);
    ssize_t len;
    while ((ssize_t) data.find("\r\n\r\n") < 0 && (len = read(clfd, &buf[0], buf.size())) > 0) {
      data.append(&buf[0], len);
    }

    auto req = parse::request(std::istringstream(data));
    // end get request

    static_file = "public";
    controller::load(req); // load css, images, ... 

    std::cout << "request: " << req[0] << " " << req[1] << '\n';
    // get
    middleware(route::get("/", req, controller::redirect_index));
    middleware(route::get("/index.html", req, controller::render_index));
    middleware(route::get("/info.html", req, controller::render_info));
    middleware(route::get("/404.html", req, controller::render_404));
    middleware(route::get("*", req, controller::redirect_404));

    // post
    middleware(route::post("/index.html", req, controller::auth));
  }

  void _socket() {
    if ((svfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
  	  perror("socket failed");
  	  exit(1);
  	}
  }

  void _setsockopt() {
    if (setsockopt(svfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
  	  perror("setsockopt failed");
  	  exit(1);
  	}
  }

  void _bind() {
    if (bind(svfd, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
  	  perror("bind failed");
  	  exit(1);
  	}
  }

  void _listen() {
    if (listen(svfd, 5) < 0) {
  	  perror("listen failed");
  	  exit(1);
  	}
	std::cout << "listening on port 80..." << '\n';
  }

  void _accept() {
    while (1) {
  	  if ((clfd = accept(svfd, 0, 0)) > 0) {
        std::cout << clfd << '\n';
        std::thread t(conn);
        t.join();
        close(clfd); 
      }
  	}
  }

  void _close() {
    close(svfd);
  }
}