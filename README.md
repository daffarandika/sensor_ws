# ITS Robocon ROS2 CI C++

Repository ini berisi workflow yang digunakan untuk menjalankan Continuous Integration (CI) pada package ROS 2 menggunakan GitHub Actions.

CI ini akan otomatis:

* Build package ROS2
* Menjalankan unit test
* Menjalankan linter C++ dan XML

Ketika ada pull request masuk ke branch `master`

---

## Cara Menerapkan Workflow

1. Klik tombol **"Use this template"** di bagian atas halaman ini untuk membuat repositori baru.

2. Sesuaikan konfigurasi dengan cara mengubah variable berikut di file `ros2_ci.yaml`:

```yaml
env:
    PACKAGE_NAME: nama_package
    ROS2_DISTRO: humble
```

* Ganti `PACKAGE_NAME` → nama package yang sesuai
* Ganti `ROS2_DISTRO` → distro ROS2 yang mau dipakai

3. Buat branch baru, lalu commit dan push ke branch tersebut:

```bash
git checkout -b fitur/ci-setup
git add .
git commit -m "setup: menambahkan ROS2 CI workflow"
git push origin fitur/ci-setup
```

4. Buat Pull Request ke branch `master`

---

## Penjelasan Workflow

Workflow ini akan berjalan ketika:

* Ada **Pull Request ke branch `master`**
* Perubahan **bukan** pada file berikut:

  * `.md`
  * `.rst`
  * folder `docs/`
  * `LICENSE`
  * `.gitignore`

---

## Job: Build and Test ROS2

Job ini bertanggung jawab untuk build dan testing package ROS2.

### Detail:

* Menggunakan runner: `ubuntu-22.04`
* Menggunakan container:

  ```
  rostooling/setup-ros-docker:ubuntu-jammy-latest
  ```

### Steps:

1. **Checkout repository**
2. **Cache rosdep**

   * Mempercepat instalasi dependency ROS
3. **Cache colcon workspace**

   * Menyimpan hasil build (`build/` dan `install/`)
4. **Build dan test**

   * Menggunakan:

     ```
     ros-tooling/action-ros-ci@v0.4
     ```
   * Otomatis:

     * Install dependency
     * Build package
     * Jalankan test

---

## Job: Ament Lint

Job ini digunakan untuk memastikan kualitas kode sesuai standar ROS2.

### Linter yang dijalankan:

* `cpplint` → standar penulisan C++
* `uncrustify` → formatting kode
* `xmllint` → validasi file XML

### Detail:

* Menggunakan matrix strategy → semua linter dijalankan paralel
* Tidak fail-fast → semua linter tetap jalan walaupun ada yang gagal

### Steps:

1. Checkout repository
2. Setup ROS environment
3. Jalankan linter dengan:

   ```
   ros-tooling/action-ros-lint@v0.1
   ```

---

## Struktur Cache

Untuk mempercepat proses CI, digunakan cache:

### 1. rosdep cache

```
~/.ros/rosdep
```

### 2. colcon cache

```
~/ros_ws/build
~/ros_ws/install
```

---

## Contoh Alur Penggunaan

1. Developer membuat branch baru
2. Melakukan perubahan pada package ROS2
3. Push ke repository
4. Membuat Pull Request ke `master`
5. GitHub Actions akan otomatis:

   * Build package
   * Menjalankan test
   * Menjalankan linter
6. Jika semua job sukses → PR siap di-merge 
