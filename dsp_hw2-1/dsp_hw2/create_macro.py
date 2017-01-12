f = open('lib/pretrain_macros', 'w')

num1 = 1000
num2 = 100
num3 = 1000
num4 = 351
num5 = 100
num6 = 1000

f.write('~o\n')
f.write('<STREAMINFO> 1 39\n')
f.write('<VECSIZE> 39<MFCC_0_D_A_Z>\n')

f.write('~V \"layer2_bias\"\n')
f.write('<VECTOR> ' + str(num1) +  '\n')
for i in range(num1):
    f.write('0.0 ')
f.write('~V \"layerout_bias\"\n')
f.write('<VECTOR> ' + str(num2) + '\n')
for i in range(num2):
    f.write('0.0 ')
f.write('~M \"layer2_weight\"\n')
f.write('<MATRIX> ' + str(num3) + str(num4) + '\n')
for i in range(num3):
    for j in range(num4):
        f.write('0.0 ')
    f.write('\n')

f.write('~M \"layerout_weight\"\n')
f.write('<MATRIX> ' + str(num5) + str(num6) + '\n')
for i in range(num5):
    for j in range(num6):
        f.write('0.0 ')
    f.write('\n')

