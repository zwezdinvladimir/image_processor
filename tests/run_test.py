import os, pathlib

applied_filters = { #result name: applied filters
    "gs": "-gs",
    "neg": "-neg",
    "sharp": "-sharp",
    "edge": "-edge 0.5",
    "blur": "-blur 1.0",
    "quad": "-quad 0.005"
}
input_folder = pathlib.Path("tests/input")
output_folder = pathlib.Path("tests/output")
os.system(f"rm -rf {output_folder} && mkdir {output_folder}")
for result_name, filter in applied_filters.items():
    for input_image in input_folder.glob("*.bmp"):
        output_image = output_folder / f"{input_image.stem}_{result_name}.bmp"
        os.system(f"./build/image_processor {input_image} {output_image} {filter}")
        print(f"Running {input_image.name} -> {output_image.name} with filter {filter}")
