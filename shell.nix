{ pkgs ? import <nixpkgs> {} }:
  pkgs.mkShell {
    name = "ripsum-dev";

    nativeBuildInputs = with pkgs.buildPackages; [ 
      emacs
      git
      cmake
      gcc
      openssl
      clang-tools
    ];
}


