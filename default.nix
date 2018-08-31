let
  pkgs = import <nixpkgs> {};
in
{stdenv ? pkgs.stdenv}:

with pkgs; stdenv.mkDerivation rec {
  name = "necroquake";

  buildInputs = [
    bashInteractive

    which
    pkgconfig

    openal
    SDL
  ];

  enableParallelBuilding = true;
}
