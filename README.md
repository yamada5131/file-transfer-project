# Hướng dẫn chạy chương trình chia sẻ file

## 1. **Chuẩn bị môi trường**

Trước khi bắt đầu, bạn cần chắc chắn rằng máy tính của bạn đã cài đặt những phần mềm sau:

- Trình biên dịch GCC (`gcc`)
- Công cụ quản lý dự án `make`

Nếu chưa có, bạn có thể cài đặt trên Ubuntu bằng lệnh:

```bash
sudo apt-get install build-essential
```

## 2. **Cấu trúc dự án**

Đảm bảo rằng dự án của bạn có cấu trúc như sau:

```
file_sharing_app/
|-- client.c        # Mã nguồn client
|-- server.c        # Mã nguồn server
|-- user.c          # Mã nguồn xử lý người dùng
|-- user.h          # Header file cho quản lý người dùng
|-- Makefile        # File điều khiển việc biên dịch
|-- users.txt       # File lưu trữ thông tin người dùng (sẽ tự tạo sau khi chạy server)
|-- file_ops.c      # Thực hiện các thao tác liên quan đến upload/download file.
|-- file_ops.h
|-- folder_ops.c    # Thực hiện các thao tác trên thư mục (tạo, xóa, di chuyển,...).
|-- folder_ops.h
|-- permissions.c   # Quản lý quyền truy cập tệp/thư mục.
|-- permissions.h
|-- search.c        # Thực hiện các chức năng tìm kiếm file.
|-- search.h
```

## 3. **Hướng dẫn biên dịch**

Trước khi chạy chương trình, bạn cần biên dịch mã nguồn. Sử dụng lệnh sau trong thư mục dự án:

```bash
make
```

Lệnh này sẽ biên dịch cả `client` và `server`. Nếu không có lỗi gì, bạn sẽ thấy các file thực thi (`server` và `client`) được tạo ra.

## 4. **Chạy chương trình**

### 4.1. **Chạy Server**

Mở một cửa sổ terminal và chạy lệnh sau để khởi động server:

```bash
./server
```

Khi chạy thành công, server sẽ lắng nghe các yêu cầu từ client.

### 4.2. **Chạy Client**

Mở một cửa sổ terminal khác và chạy client:

```bash
./client
```

Client sẽ kết nối tới server, cho phép bạn nhập các lệnh để đăng ký hoặc đăng nhập.

## 5. **Cách sử dụng**

### Đăng ký tài khoản mới

Trên client, bạn có thể nhập lệnh sau để đăng ký tài khoản mới:

```bash
register
```

Sau đó, client sẽ yêu cầu bạn nhập tên tài khoản và mật khẩu:

```bash
Enter username: [nhập username của bạn]
Enter password: [nhập password của bạn]
```

Nếu đăng ký thành công, bạn sẽ nhận được thông báo từ server:

```bash
Server: Registration successful.
```

### Đăng nhập

Để đăng nhập, bạn nhập lệnh:

```bash
login
```

Client sẽ yêu cầu bạn nhập tên tài khoản và mật khẩu:

```bash
Enter username: [nhập username của bạn]
Enter password: [nhập password của bạn]
```

Nếu đăng nhập thành công, bạn sẽ thấy thông báo:

```bash
Server: Login successful.
```

Nếu không đúng, server sẽ phản hồi thông báo lỗi.

## 6. **Làm sạch dự án**

Nếu muốn xóa các file thực thi và các file `.o`, bạn có thể chạy lệnh sau:

```bash
make clean
```

Lệnh này sẽ xóa các file biên dịch trung gian và file thực thi (`server` và `client`).

## 7. **Lưu ý**

- **File `users.txt`**: File này sẽ được tạo ra khi bạn bắt đầu đăng ký người dùng. Nó sẽ lưu trữ các thông tin đăng ký (username và hashed password) của tất cả người dùng.
- **Giới hạn nhập liệu**: Tên đăng nhập và mật khẩu được giới hạn ở 256 ký tự để tránh tràn bộ nhớ.
