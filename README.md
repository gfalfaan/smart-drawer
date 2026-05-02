# Smart Drawer
Smart Drawer adalah sistem penyimpanan komponen yang dilengkapi web interface dan LED indikator untuk membantu mencari dan mengelola komponen dengan lebih mudah sistem ini dirancang dengan menggunakan web server sebagai interface pencarian barang laci.
## Fitur
* Manajemen inventori lewat web
* Pencarian laci dengan bantuan LED
* Kontrol LED (warna, efek, brightness)
* Jam digital plus setup lampu kamar dan alarm ( Sebenernya Tambahan Aja karena Iseng )
* Bisa tambahkan wifi saat tidak mendeteksi wifi yang sesuai maka akan otomatis berubah acces_point
* Komunikasi antar mikrokontroler (ESP32, ATmega, Arduino Nano) pakai RX/TX ( Kenapa ribet pakek banyak mikro ? karena atmega sama nano ga kepakek wkwkwk iseng aja tambahin fitur diproject )
## Cara Pakai
1. Pertama upload aja dulu, ubah aja gapapa programnya ga ada yang larang huhu
2. Cara penggunaan smart drawernya nih
3. Colokkan ESP32 , pasti dong
4. Saat esp tidak mendeteksi wifi maka otomatis akan menjadi acces point
5. Wifi esp nya ada diprogram hehe
6. Buka di browser bisa pakai DNS local tertera dibawah :
   * `http://smartdrawer.local`
   * atau `192.168.4.1`
7. saat sudah masuk ke web , diharapkan login dengan wifi sendiri agar gambarnya muncul karena biar gambarnya muncul aja wkwkwk soalnya gambar inventori berdasarkan link web jadi harus ada koneksi internet
8. oh ya format file inventori json dan ada di pengaturan jadi ga perlu takut ilang deh , kalau udah buat kumpulan inventaris . kemudian disimpan di esp32 sih hehehe makane simpen json nya
9. kalau mau atur jam digital ya tinggal atur ada di pengaturan
10. btw arduino nano nya cuma nerima perintah dari rtc atmega lewat esp32 jadi jika error ga nampilin apa apa berarti format mu salah atau atmega ga ngirim apa apa
## Struktur
* `firmware/esp32` → program utama + web server
* `firmware/atmega` → jam & alarm
* `firmware/arduinonano` → display panel
## Catatan
Project ini dibuat untuk membantu menyimpan dan mencari komponen elektronik agar lebih rapi dan efisien.
