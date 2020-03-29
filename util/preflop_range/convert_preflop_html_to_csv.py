import os 

coresp = {2: '2', 3: '3', 4: '4', 5: '5', 6: '6', 7: '7', 8: '8', 9: '9', 10: 'T', 11: 'J', 12: 'Q', 13: 'K', 14: 'A'}

for filename in os.listdir('html'):
    lines = open(os.path.join('html', filename)).readlines() 
    in_table = False
    card_lhs = 14
    csv_buffer = ''
    for line in lines:
        if '</table>' in line:
            break
        if '<table>' in line:
            in_table = True
            continue
        if not in_table:
            continue
        # stripping <tr>, </tr>
        line = line[7:-6]
        line = line.split('</td>')
        for action, card_rhs in zip(line, reversed(range(2, 15))):
            if '"r"' in action:
                action_abs = 'R'
            elif '"c"' in action:
                action_abs = 'C'
            else:
                action_abs = 'F'
            raise_prob = 0
            try:
                raise_prob = int(action[-1])
                raise_prob = int(action[-2]) * 10 + raise_prob
            except:
                pass
            if action_abs == 'R':
                raise_prob = 100
            csv_buffer_line = f'{action_abs},{raise_prob}\n'
            if action_abs == 'F' and raise_prob == 0:
                continue
            if card_lhs > card_rhs:
                csv_buffer_line = f'{coresp[card_lhs]}{coresp[card_rhs]}s,{csv_buffer_line}'
            else:
                csv_buffer_line = f'{coresp[card_rhs]}{coresp[card_lhs]}o,{csv_buffer_line}'
            csv_buffer += csv_buffer_line
        card_lhs -= 1
    open(os.path.join('csv', f'{filename[:-5]}.csv'), 'w').write(csv_buffer)
