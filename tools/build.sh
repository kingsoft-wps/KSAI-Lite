#!/bin/bash

set -e
set -x

case "$1" in 
  linux)
    echo "build for linux platform"
    case "$2" in
      native)
        /bin/bash ./build_linux.sh native
		    ;;
      cross_aarch64)
        /bin/bash ./build_linux.sh cross_aarch64
		    ;;
      cross_mips64)
        /bin/bash ./build_linux.sh cross_mips64
        ;;
      all)
        echo "target arch is native, aarch64 and mips64" 
        /bin/bash ./build_linux.sh native
        /bin/bash ./build_linux.sh cross_aarch64
        /bin/bash ./build_linux.sh cross_mips64 
        ;;
      *)
        echo "please input the target arch you want to build" 
        ;;
    esac 
    ;;

  android)
    echo "build for android platform"
    /bin/bash ./build_android.sh 
    ;;

  all)
    echo "build for all platform, including windows linux and android"
    /bin/bash ./build_android.sh 

    /bin/bash ./build_linux.sh native
    /bin/bash ./build_linux.sh cross_aarch64
    /bin/bash ./build_linux.sh cross_mips64 
    ;;
  *)
    echo "please input the platform you want to build" 
    ;;

esac

