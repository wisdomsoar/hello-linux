my_root=$(pwd)

find . -type l -ls | grep mnt  | while read one_line; do

    #   link_file => link_to_file
        #       |
        #       V
        # full path: link_full_path

    link_file=$(echo $one_line | awk '{print $11}')
    echo "link_file: "$link_file

    link_to=$(echo $one_line | awk '{print $13}')
    echo "link_to_file: "$link_to 

    link_full_path=$( realpath -s $link_file)
    echo "link_full_path: "$link_full_path

    link_folder=$(dirname $link_full_path)
    link_file_name=$(basename $link_full_path)

    cd $link_folder
    relative_link_to=$(realpath --relative-to="$(pwd)" $link_full_path)
    echo "current folder: "$(pwd)
    echo "relative_link_to: "$relative_link_to
    echo "link_file_name: "$link_file_name

    answer=$1
    if [ "$answer" == "--force" ]; then
        echo "you used --force"
    else
        echo "are you sure (--force/n): ln -sf $relative_link_to $link_file_name?"
        read  answer </dev/tty
    fi

    if [ "$answer" == "--force" ]; then
      echo "execute..."
      set -x
      rm $link_file_name
      ln -sf $relative_link_to $link_file_name
      set +x
    else
      echo "skip..."
    fi

    cd $my_root
    echo "" && echo "" && echo "" && echo ""

done
