#!/bin/bash

set -e


echo "creating h1 namespace with ip 10.10.10.1/24"
sudo ip netns add h1
sudo ip link add h1-ns type veth peer name h1-sw
sudo ip link set netns h1 h1-ns
sudo ip netns exec h1 ip addr add 10.10.10.1/24 dev h1-ns
sudo ip netns exec h1 ifconfig h1-ns up


echo "creating h2 namespace with ip 10.10.10.2/24"
sudo ip netns add h2
sudo ip link add h2-ns type veth peer name h2-sw
sudo ip link set netns h2 h2-ns
sudo ip netns exec h2 ip addr add 10.10.10.2/24 dev h2-ns
sudo ip netns exec h2 ifconfig h2-ns up


echo "creating s1 namespace as simple repeater"
sudo ip netns add s1
sudo ip link set netns s1 h1-sw
sudo ip link set netns s1 h2-sw
sudo ip netns exec s1 ifconfig h1-sw up
sudo ip netns exec s1 ifconfig h2-sw up