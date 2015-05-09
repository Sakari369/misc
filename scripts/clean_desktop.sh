#!/bin/bash

DESKTOP_DIR="${HOME}/Desktop/"

AUDIO_FILES="*.aiff *.aif *.wav *.mp3 *.ogg *.flac *.m4a"
AUDIO_DIR="${HOME}/Music"

MOVIE_FILES="*.mov *.mp4 *.mkv *.avi"
MOVIE_DIR="${HOME}/Movies"

PICTURES_FILES="*.psd *.tiff *.png *.bmp *.jpg *.jpeg *.gif *.ptg"
PICTURES_DIR="${HOME}/Pictures"

DOCUMENT_FILES="*.zip *.doc *.odx *.pdf *.txt"
DOCUMENTS_DIR="${HOME}/Documents"

cd ${DESKTOP_DIR}
mv -v ${AUDIO_FILES} ${AUDIO_DIR}
mv -v ${MOVIE_FILES} ${MOVIE_DIR}
mv -v ${PICTURES_FILES} ${PICTURES_DIR}
mv -v ${DOCUMENT_FILES} ${DOCUMENTS_DIR}
