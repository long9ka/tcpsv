# Chạy Chương trình
* `make install` để biên dịch
* `sudo make run` để chạy (bind yêu cầu phải cấp quyền, nên có sudo)
* nếu muốn nhanh thì `sudo make` (vừa install vừa run)
* muốn xoá như ban đầu thì `sudo make clean`

> Dùng máy linux hoặc wsl của windows, gg cách cài wsl cũng nhanh, cài xong rồi git clone -> sudo make
# Sơ qua cái code
```c++
int main() {
  serv::_socket();
  serv::_setsockopt();
  serv::_bind();
  serv::_listen();
  serv::_accept();
  serv::_close();
  return 0;
}
```
-  serv` là `namespace` chứa các hàm `_socket(), _bind(), ... `

- cái namespace đó trong thư viện `<tcpsock.h>` 
- xử lí `thread` trong hàm _accept(), hàm xử lí chính là `conn()`
 ```c++
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
  ```