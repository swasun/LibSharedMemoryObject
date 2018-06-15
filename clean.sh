#!/bin/bash

echo "Removing bin release files"
rm -rf bin/release/*
echo "Removing bin debug files"
rm -rf bin/debug/*
echo "Removing build release files"
rm -rf build/release/*
echo "Removing build debug files"
rm -rf build/debug/*
echo "Removing out files"
rm -rf out/*
echo "Done"
