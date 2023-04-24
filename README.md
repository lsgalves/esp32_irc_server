# ESP32 IRC Server

An IRC server to run in a [ESP32-S3 with TFT Display](https://github.com/Xinyuan-LilyGO/T-Display-S3).

> :construction: Under development

## Description

This project is built with no thought of using it in production, I just did it for fun at the office.

## Usage

It is necessary to change the following constants and variables with the values corresponding to their name:

```c
#define APP_NAME "example"
#define ADMIN_PASS "<PUT-YOUR-ADMIN-PASSWORD>"
#define WIFI_SSID "<PUT-YOUR-SSID>"
#define WIFI_PASS "<PUT-YOUR-WIFI-PASSWORD>"

// Put your default channels to array
String channels[] = {"example"};
```

I also removed 2 files, `logo.h` (109x63) and `minilogo.h` (41x24) for copyright reasons.
Had uploaded images using the following [converter](http://www.rinkydinkelectronics.com/t_imageconverter565.php). If you prefer, feel free to change the sizes of your images.

## License

This project is under the MIT license. See the [LICENSE](./LICENSE) file for details.

