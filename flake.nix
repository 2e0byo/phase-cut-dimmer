{
  description = "Pic experiments.";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    oprog = {
      url = "github:2e0byo/open-pic-prog";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    xc8 = {
      url = "https://ww1.microchip.com/downloads/aemDocuments/documents/DEV/ProductDocuments/SoftwareTools/xc8-v4.00-full-install-linux-x64.tar.xz";
      flake = false;
    };
    dfp = {
      url = "https://packs.download.microchip.com/Microchip.PIC10-12Fxxx_DFP.1.10.193.atpack";
      flake = false;
      type = "tarball";
    };
  };

  outputs = {
    self,
    nixpkgs,
    oprog,
    xc8,
    dfp,
  }: let
    system = "x86_64-linux";
    pkgs = import nixpkgs {
      inherit system;
      config.allowUnfree = true;
    };
    xc8-cc = pkgs.writeShellScriptBin "xc8-cc" ''
      ${xc8}/bin/xc8-cc "$@" -mdfp="${dfp}/xc8"
    '';
    # xc8-cc = pkgs.callPackage ./xc8 {inherit xc8 dfp;};
    xc8-clangd = pkgs.writeShellScriptBin "clangd" ''
      ${xc8}/microchip/xc8/v3.10/bin/xc8-clangd "$@"
    '';
  in {
    packages.${system}.xc8 = xc8-cc;
    devShells.${system}.default = pkgs.mkShell {
      packages = with pkgs;
        [
          gnumake
          oprog.packages.${system}.default
          (python3.withPackages (ps: with ps; [ipython]))
          gcc
          clang-tools
          microcom
        ]
        ++ [
          xc8-cc
          # xc8-clangd
        ];
    };
  };
}
