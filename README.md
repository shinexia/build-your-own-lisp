# Build Your Own Lisp

labs of <http://www.buildyourownlisp.com/contents>

## Dependencies

Editline

``` bash
# ubuntu
sudo apt-get install libedit-dev flex bison
# centos
sudo yum install libedit-devel flex bison
```

## build

``` bash
git submodule update --init
make -j$(nproc)
```
