
# test string = "69 6D 67 33 2E 6A 61 76 6D 6F 73 74 2E 63 6F 6D"
display_video_filenames() {
    folder="$1"
    video_extensions=("mp4" "avi" "mkv" "mov") # Add more video file extensions as needed
    video_files=()
    for ext in "${video_extensions[@]}"; do
        while IFS= read -r -d '' file; do
            video_files+=("$file")
        done < <(find "$folder" -type f -name "*.$ext" -maxdepth 1  -print0)
    done

    if [[ ${#video_files[@]} -eq 0 ]]; then
        echo "No video files found in $folder" > /dev/null
    else
        my_pwd=$(pwd)
        cd $folder
        echo "Video files in $folder:"
        for file in "${video_files[@]}"; do
            echo "$file"
            #read name 
            read -p "Please enter your name: " name </dev/tty
            echo "your name: "$name
            if [[ -n "$name" ]]; then
                wget "https://xxx/images/$name.webp"
            fi
        done
        cd $my_pwd
    fi
}

read_text_file() {
    file="$1"
    while IFS= read -r line; do
        display_video_filenames "$line"
    done < "$file"
}

# Usage: ./download_videos.sh input.txt
if [[ $# -ne 1 ]]; then
    echo "Usage: $0 input.txt"
    exit 1
fi

input_file="$1"

if [[ ! -f "$input_file" ]]; then
    echo "Error: Input file does not exist."
    exit 1
fi

read_text_file "$input_file"
