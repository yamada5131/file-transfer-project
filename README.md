# Dự Án File Transfer

## **Giới Thiệu**

Dự án **File Transfer** là một ứng dụng mạng đơn giản được phát triển bằng ngôn ngữ lập trình C, cho phép người dùng đăng ký, đăng nhập, và thực hiện các thao tác tải lên (upload) và tải xuống (download) file từ server. Ứng dụng này bao gồm hai thành phần chính:

- **Server**: Quản lý người dùng, xử lý yêu cầu từ client, lưu trữ và phân phối file.
- **Client**: Giao diện người dùng để tương tác với server, thực hiện các chức năng đăng ký, đăng nhập, upload, và download file.

## **Tính Năng**

- **Đăng ký người dùng mới**: Người dùng có thể tạo tài khoản mới bằng cách cung cấp tên đăng nhập và mật khẩu.
- **Đăng nhập**: Xác thực người dùng đã đăng ký để truy cập các chức năng bảo mật như upload và download file.
- **Upload file**: Người dùng có thể tải lên các file từ máy tính của mình lên server.
- **Download file**: Người dùng có thể tải xuống các file từ server về máy tính của mình.
- **Quản lý file theo người dùng**: Mỗi người dùng có thư mục riêng trên server để lưu trữ file của mình.

## **Cấu Trúc Thư Mục**

```
file-transfer-project/
├── client/
│   ├── client.c           --- Source code chính của client
│   ├── client.h           --- Header file cho client
│   ├── command.c          --- Xử lý các lệnh từ người dùng (đăng ký, đăng nhập, upload, download)
│   ├── command.h          --- Header file cho các hàm xử lý lệnh
│   ├── main.c             --- Hàm main của client
│   ├── Makefile           --- File cấu hình để biên dịch client
│   ├── session.c          --- Quản lý phiên làm việc của client
│   ├── session.h          --- Header file cho session
├── common/
│   ├── CJSON.C            --- Mã nguồn của thư viện cJSON
│   ├── cJSON.h            --- Header file của thư viện cJSON
│   ├── common.h           --- Header file chung cho cả client và server
│   ├── Makefile           --- File cấu hình để biên dịch common
│   ├── network.c          --- Xử lý các chức năng mạng như gửi và nhận thông điệp qua socket
│   ├── network.h          --- Header file cho network
│   ├── protocol.c         --- Xử lý giao thức truyền thông giữa client và server, mã hóa và giải mã JSON
│   └── protocol.h         --- Header file cho protocol
├── data/
│   └── (Thư mục lưu trữ file của người dùng) --- Mỗi người dùng có một thư mục riêng với tên đăng nhập của họ
├── server/
│   ├── data_manager.c     --- Quản lý dữ liệu người dùng, thêm người dùng mới, kiểm tra tồn tại người dùng
│   ├── data_manager.h     --- Header file cho data_manager
│   ├── handler.c          --- Xử lý các loại yêu cầu từ client như REGISTER, LOGIN, UPLOAD, DOWNLOAD
│   ├── handler.h          --- Header file cho handler
│   ├── main.c             --- Hàm main của server
│   ├── Makefile           --- File cấu hình để biên dịch server
│   ├── server.c           --- Chứa các hàm chính của server, quản lý kết nối và xử lý yêu cầu từ client
│   ├── server.h           --- Header file cho server
│   ├── session.c          --- Quản lý phiên làm việc của mỗi client trên server
│   ├── session.h          --- Header file cho session
│   └── users.txt          --- Tệp tin lưu trữ thông tin người dùng đã đăng ký (tên đăng nhập và mật khẩu)
├── Makefile               --- File cấu hình để biên dịch toàn bộ dự án
└── README.md              --- Tài liệu hướng dẫn dự án
```

## **Yêu Cầu Hệ Thống**

- **Hệ điều hành**: Linux/Unix
- **Ngôn ngữ lập trình**: C (phiên bản GCC 14.2.1)
- **Thư viện**:
  - `cJSON`: Thư viện xử lý JSON trong C. [CJSON GitHub](https://github.com/DaveGamble/cJSON)
- **Các công cụ**:
  - `make`: Để biên dịch dự án.
  - `gcc`: Trình biên dịch C.

## **Cài Đặt**

### **1. Biên Dịch Dự Án**

Di chuyển đến folder /file-transfer-project, sử dụng `make` để biên dịch dự án.

```bash
cd /path/to/file-transfer-project/
make
```

### **2. Thiết Lập Thư Mục Dữ Liệu**

Thư mục `data/` nằm cùng cấp với thư mục `server/` và `client/`. Mỗi người dùng sau khi đăng ký sẽ có một thư mục riêng trong `data/` với tên đăng nhập của họ.

```bash
cd /path/to/file-transfer-project/
mkdir data
```

## **Sử Dụng**

### **1. Khởi Động Server**

Chuyển đến thư mục `server/` và chạy server trên một cổng cụ thể (ví dụ: 12345).

```bash
cd /path/to/file-transfer-project/server/
./server 12345
```

**Log Server:**

```
Thư mục hiện tại của server: /path/to/file-transfer-project/server
Server đang lắng nghe trên cổng 12345
Kết nối mới từ 127.0.0.1:<port>
Bắt đầu xử lý client.
...
```

### **2. Khởi Động Client**

Mở một terminal mới, chuyển đến thư mục `client/` và chạy client.

```bash
cd /path/to/file-transfer-project/client/
./client
```

**Giao Diện Client:**

```
Menu =
1. Đăng ký
2. Đăng nhập
3. Upload file
4. Download file
5. Thoát
Lựa chọn của bạn:
```

### **3. Đăng Ký Người Dùng Mới**

1. Chọn tùy chọn `1` để đăng ký.
2. Nhập tên đăng nhập và mật khẩu khi được yêu cầu.

**Ví dụ:**

```
Lựa chọn của bạn: 1
Nhập tên đăng nhập: user1
Nhập mật khẩu: pass123
Đăng ký thành công.
```

### **4. Đăng Nhập**

1. Chọn tùy chọn `2` để đăng nhập.
2. Nhập tên đăng nhập và mật khẩu khi được yêu cầu.

**Ví dụ:**

```
Lựa chọn của bạn: 2
Nhập tên đăng nhập: user1
Nhập mật khẩu: pass123
Đăng nhập thành công.
```

### **5. Upload File**

1. Chọn tùy chọn `3` để upload file.
2. Nhập đường dẫn tới file trên máy tính của bạn để upload.

**Ví dụ:**

```
Lựa chọn của bạn: 3
Nhập đường dẫn tới file cần upload: /home/user/documents/file.txt
Đã upload file thành công.
```

### **6. Download File**

1. Chọn tùy chọn `4` để download file.
2. Nhập tên file bạn muốn download (file phải tồn tại trong thư mục `data/<username>/` trên server).

**Ví dụ:**

```
Lựa chọn của bạn: 4
Nhập tên file cần download: file.txt
Đã download file thành công.
```

**Lưu Ý:**

- **Thư mục lưu trữ file trên server**: `data/<username>/`
- **Thư mục lưu trữ file trên client**: Thư mục hiện tại nơi bạn chạy chương trình client.

## **Thêm Thông Tin**

- **Ngôn ngữ lập trình**: C
- **Phiên bản GCC**: 14.2.1
- **Thư viện phụ thuộc**: `cJSON` để xử lý JSON.
- **Cấu trúc dự án**:
  - **Client**: Xử lý giao diện người dùng và giao tiếp với server.
  - **Server**: Xử lý yêu cầu từ client, quản lý dữ liệu người dùng và file.
  - **Common**: Các thành phần chung được sử dụng bởi cả client và server như giao thức mạng, xử lý JSON.
  - **Data**: Lưu trữ file của người dùng trên server.

## **Liên Hệ**

Nếu bạn gặp bất kỳ vấn đề nào hoặc cần hỗ trợ thêm, vui lòng liên hệ qua email: [nguyenleson0002@gmail.com](mailto:nguyenleson0002@gmail.com)
