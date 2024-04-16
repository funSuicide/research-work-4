START_TABLE = [[12, 4, 6, 2, 10, 5, 11, 9, 14, 8, 13, 7, 0, 3, 15, 1],
               [6, 8, 2, 3, 9, 10, 5, 12, 1, 14, 4, 7, 11, 13, 0, 15],
               [11, 3, 5, 8, 2, 15, 10, 13, 14, 1, 7, 4, 12, 9, 6, 0],
               [12, 8, 2, 1, 13, 4, 15, 6, 7, 0, 10, 5, 3, 14, 9, 11],
               [7, 15, 5, 10, 8, 1, 6, 13, 0, 9, 3, 14, 11, 4, 2, 12],
               [5, 13, 15, 6, 9, 2, 12, 10, 11, 7, 8, 1, 4, 3, 14, 0],
               [8, 14, 2, 5, 6, 9, 1, 12, 15, 4, 11, 0, 13, 10, 3, 7],
               [1, 7, 14, 13, 0, 5, 8, 3, 4, 15, 10, 6, 9, 12, 11, 2]]


# START_TABLE = [[1, 7, 14, 13, 0, 5, 8, 3, 4, 15, 10, 6, 9, 12, 11, 2],
#                [8, 14, 2, 5, 6, 9, 1, 12, 15, 4, 11, 0, 13, 10, 3, 7],
#                [5, 13, 15, 6, 9, 2, 12, 10, 11, 7, 8, 1, 4, 3, 14, 0],
#                [7, 15, 5, 10, 8, 1, 6, 13, 0, 9, 3, 14, 11, 4, 2, 12],
#                [12, 8, 2, 1, 13, 4, 15, 6, 7, 0, 10, 5, 3, 14, 9, 11],
#                [11, 3, 5, 8, 2, 15, 10, 13, 14, 1, 7, 4, 12, 9, 6, 0],
#                [6, 8, 2, 3, 9, 10, 5, 12, 1, 14, 4, 7, 11, 13, 0, 15],
#                [12, 4, 6, 2, 10, 5, 11, 9, 14, 8, 13, 7, 0, 3, 15, 1]]


def get_table_4x256():
    result = [[0 for i in range(256)] for j in range(4)]
    for i in range(4):
        for j in range(256):
            # result[i].append(START_TABLE[2 * i][j & 0xf])
            result[i][j] = (START_TABLE[2 * i][j & 0xf]) | (START_TABLE[2 * i + 1][j >> 4] << 4)
    return result


def get_table_2x65536(table):
    result = [[0 for i in range(256 * 256)] for j in range(2)]
    for i in range(2):
        for j in range(65536):
            # result[i].append(table[2 * i][j & 0xff])
            result[i][j] = table[2 * i][j & 0xFF] | (table[2 * i + 1][j >> 8] << 8)
    return result


def create_file(table, filename, signature):
    with open(filename, 'w') as f:
        f.write('#pragma once\n\n')
        f.write(signature + ' = {\n')
        for i in range(len(table)):
            f.write('\t\t\t\t{ ')
            for j in range(len(table[0])):
                f.write(str(table[i][j]))
                if j != len(table[0]) - 1:
                    f.write(', ')
            f.write(' },\n')
        f.write('};')


def check(table):
    for i in range(len(table)):
        tmp = set(table[i])
        if len(tmp) != len(table[i]):
            return False
    return True


def check2(table256, table65535):
    for i in range(256):
        val1 = table256[0][i]
        val2 = table256[1][i]

        val3 = table65535[0][i]

        if (val3 != ((val2 << 8) | val1)):
            return 0

    return 1


def main():
    table_4x256 = get_table_4x256()
    print(check(table_4x256))
    table_2x65536 = get_table_2x65536(table_4x256)
    print(check(table_2x65536))
    print(check2(table_4x256, table_2x65536))
    create_file(table=table_4x256, filename='table4X256.hpp', signature='static constexpr uint8_t sTable4x256[4][256]')
    create_file(table=table_2x65536, filename='table2X65536.hpp',
                signature='static constexpr uint16_t sTable2x65536[2][1<<16]')
    return 0


if __name__ == '__main__':
    main()
