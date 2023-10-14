gcc agent.c -o agent.out
gcc controller.c -o controller.out

gnome-terminal --title="AGENT" -- bash -c "./agent.out; exec bash" &

echo "Waiting 2 seconds for agent to run..."
sleep 2

gnome-terminal --title="CONTROLLER" -- bash -c "./controller.out; exec bash" &
