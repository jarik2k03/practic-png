if [ "$#" -ne 1 ]; then
  echo "Usage: $0 <\"debug/release\">" && exit
fi

if [ "$1" = "debug" ]; then
  find build/debug/ -type f -name "*.pcm" | xargs du -sh | sort -h
elif [ "$1" = "release" ]; then
  find build/release/ -type f -name "*.pcm" | xargs du -sh | sort -h
else
  echo "Wrong first argument!" && exit
fi
