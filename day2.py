import sys


nred = 12
ngreen = 13
nblue = 14

game_id_sum = 0

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

        if ball_freq["red"] <= nred and ball_freq["green"] <= ngreen and ball_freq["blue"] <= nblue:
            game_id_sum += game_id_i


print("sum: ", game_id_sum)
