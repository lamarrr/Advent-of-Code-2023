import sys

power_sum = 0

with open(sys.argv[1]) as file:
    for game_id, sets in (line.split(":") for line in file.read().splitlines()):
        game_id_i = int(game_id[len("Game "):])
        ball_freq = {
            "red": 0,
            "green": 0,
            "blue": 0
        }
        for set in sets.split(";"):
            for entry in set.split(","):
                ball_count, ball_color = entry.strip().split(" ")
                ball_freq[ball_color] = max(
                    ball_freq[ball_color], int(ball_count))

        power_sum += ball_freq["red"]*ball_freq["green"]*ball_freq["blue"]


print("power sum: ", power_sum)
