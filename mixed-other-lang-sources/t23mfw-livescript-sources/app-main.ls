amreq = require 'requirejs'

amreq.config do
    base-url: 'lib' # process.cwd() + "/lib" # 'lib'

#    define-schemas
#   connect

{exec}, MT4, musts <- amreq <[
    child_process
    nodejs-MT4-bridge
    wpn/must-haves
]>

musts.globalize musts

_D 'create MT4-interface'
err <-! MT4.create
_D 'done created MT4-interface'
die err if err

