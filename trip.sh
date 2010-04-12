#!/bin/bash
mv board/phy3250 .
rm board/* -rf
mv phy3250 board/

mv cpu/arm926ejs/ ./
rm -rf cpu/*
mv arm926ejs cpu/

rm -rf doc nand_spl onenand_ipl CHANGELOG* COPYING CREDITS MAINTAINERS README

mv lib_{arm,generic} api/
rm -rf lib_*
mv api/lib_{arm,generic} .

mv include/asm-arm .
rm -rf include/asm-*
mv asm-arm include/

mv {{arm_,}config,rules}.mk tools
rm *.mk
mv tools/{{arm_,}config,rules}.mk .

