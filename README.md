# Rain Checker

Rain Checkerは、Wi-Fiモジュール(ESP-WROOM-02)とドット・マトリックスLEDを使って、Yahoo!の気象情報APIから取得した降水予測を表示するArduinoプロジェクトです。

![Rain Checker](https://raw.githubusercontent.com/keiso/rain-checker/master/image/img_2378_640x480.jpg)

(以下ドキュメントを作成中です。)

Wi-Fiモジュールは、ESPr One(旧製品名:ESP-WROOM-02 Arduino互換ボード)やESPr Developer(旧製品名:ESP-WROOM-02開発ボード)を使用すると比較的簡単に作れます。

ドット・マトリックスLEDは、アマゾンで販売されている300円前後で販売されているモジュールを使用します。MAX7219で検索すると見つかると思います。

LedControl
http://wayoda.github.io/LedControl/

Rain Checkerは、Yahoo!の気象情報APIを使用しますので、プリケーションを登録し、アプリケーションIDを取得しておいてください。

YOLP(地図):気象情報API - Yahoo!デベロッパーネットワーク
http://developer.yahoo.co.jp/webapi/map/openlocalplatform/v1/weather.html
