GPG_TTY=$(tty)
eval $(gpg --decrypt < ~/.cpsh/keys.asc )
export KEY SEC
