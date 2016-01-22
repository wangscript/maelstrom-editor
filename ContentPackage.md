# Introduction #

Content packages (.PAK) are consumed by the Vortex engine.

# Details #

Content packages are created by invoking VACP with the _pk_ command.
This will first build all content that belongs to the package, and then combine these into one big PAK file.

The .PAK file contains a header an a payload. The structure of the header is as follows:

| **Offset** | **Size** | **Description** |
|:-----------|:---------|:----------------|
| 0          | 4 Bytes  | The number of resources in this package |
| 4          | Variable-size, see description | Index table describing the location of each resource in the file. The size of each row in the table is 8 bytes, so the total size of this block is _(Number of resources in package `*` 8)_ |

Where the index table is structured as follows:
| **Offset** | **Size** | **Description** |
|:-----------|:---------|:----------------|
| 0          | 4 Bytes  | Unique resource identifier |
| 4          | 4 Bytes  | Resource file offset (relative to beginning of file) |

The payload follows directly after the header. For each resource in the file, the structure is as follows:
| **Offset** | **Size** | **Description** |
|:-----------|:---------|:----------------|
| 0          | 4 Bytes  | Size of the resource data |
| 4          | 1 Byte   | Reserved for future use. |
| 5          | Variable-size | The actual resource data |

To read the data of a specific resource, its offset needs to be found in the index table. Once this offset is found, move the file read pointer to the offset. Here, read the data size, the one byte(reserved for future use), and finally the actual data.

All information in the PAK files are stored in little-endian.