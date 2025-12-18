
PS1='${debian_chroot:+($debian_chroot)}\u@\h:\w\$ '

# If this is an xterm set the title to user@host:dir
case "$TERM" in
xterm*|rxvt*)
    PS1="\[\e]0;${debian_chroot:+($debian_chroot)}\u@\h: \w\a\]$PS1"
    ;;
*)
    ;;
esac

#Show git brach in your prompt

parse_git_branch() {
     git branch 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/ (\1)/'
}
export PS1="\u@\h \[\033[38;5;172m\]\w\[\033[38;5;203m\]\$(parse_git_branch)\[\033[00m\] $ "

export PS1='\[\033[38;5;193m\][${timer_show}s]\[\033[00m\]'$PS1

