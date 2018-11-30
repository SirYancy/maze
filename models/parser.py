fi = input("Filename ")
outname = fi.split('.')[0] + ".model"
fo = open(outname, 'w')

vertices = []
textures = []
normals = []
output = []

with open(fi) as f:
    for line in f:
        if line.startswith("vn"):
            i = line.find(' ')
            n = line[i:].strip()
            n = n.split(' ')
            normals.append(n)
        elif line.startswith("vt"):
            i = line.find(' ')
            t = line[i:].strip()
            t = t.split(' ')
            textures.append(t)
        elif line.startswith("v"):
            i = line.find(' ')
            v = line[i:].strip()
            v = v.split(' ')
            vertices.append(v)
        elif line.startswith("f"):
            i = line.find(' ')

            verts = line[i:].split(' ')
            print(verts)
            vertex0 = verts[1].split('/')
            vertex1 = verts[2].split('/')
            vertex2 = verts[3].split('/')

            v0 = int(vertex0[0]) -1
            v1 = int(vertex1[0]) -1
            v2 = int(vertex2[0]) -1

            t0 = int(vertex0[1]) -1
            t1 = int(vertex1[1]) -1
            t2 = int(vertex2[1]) -1

            n0 = int(vertex0[2]) -1
            n1 = int(vertex1[2]) -1
            n2 = int(vertex2[2]) -1

            print("Triangle: ({0},{1},{2})\nNormal: ({3},{4},{5})\n".format(v0,v1,v2,n0,n1,n2))
            print("Points: \n{0}\n{1}\n{2}\n".format(vertices[v0], vertices[v1], vertices[v2]))
            print("Normals: \n{0}\n{1}\n{2}\n".format(normals[n0], normals[n1], normals[n2]))
            print("Textures: \n{0}\n{1}\n{2}\n".format(textures[t0], textures[t1], textures[t2]))

            output.extend(vertices[v0])
            output.extend(textures[t0])
            output.extend(normals[n0])

            output.extend(vertices[v1])
            output.extend(textures[t1])
            output.extend(normals[n1])

            output.extend(vertices[v2])
            output.extend(textures[t2])
            output.extend(normals[n2])

        else:
            continue

o = "\n".join(output)
# print(output)
# print(len(output))
fo.write(str(len(output)) + "\n" + o)




