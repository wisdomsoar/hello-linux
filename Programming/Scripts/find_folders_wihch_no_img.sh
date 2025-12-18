contains_image_files() {
    folder="$1"
    image_extensions=("jpg" "jpeg" "png" "gif" "webp") # Add more image file extensions as needed
    for ext in "${image_extensions[@]}"; do
        if ls "$folder"/*."$ext" >/dev/null 2>&1; then
            return 0 # Folder contains image files
        fi
    done
    return 1 # Folder does not contain image files
}

find_folders_without_images() {
    directory="$1"
    find "$directory" -type d -not -path "*/.*" -print0 | while IFS= read -r -d '' folder; do
        if ! contains_image_files "$folder"; then
            echo "$folder"
        fi
    done
}

# Usage: ./find_folders_without_images.sh /path/to/directory

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 /path/to/directory"
    exit 1
fi

directory="$1"

if [[ ! -d "$directory" ]]; then
    echo "Error: Directory does not exist."
    exit 1
fi

find_folders_without_images "$directory"
