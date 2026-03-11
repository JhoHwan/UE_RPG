import bpy
import sys

def convert_fbx_to_obj(input_path, output_path):
    # 1. 블렌더 초기화: 기본으로 생성되는 큐브, 카메라, 라이트 등을 모두 삭제
    bpy.ops.wm.read_factory_settings(use_empty=True)
    
    # 2. FBX 파일 임포트
    try:
        bpy.ops.import_scene.fbx(filepath=input_path)
        print(f"[Success] Imported: {input_path}")
    except Exception as e:
        print(f"[Error] Failed to import FBX: {e}")
        sys.exit(1)
        
    # 3. OBJ 파일 익스포트
    try:
        # 참고: Blender 3.2 이상 버전부터는 C++ 기반의 새로운 익스포터인 wm.obj_export를 사용함.
        # 만약 구버전(3.1 이하)을 쓴다면 bpy.ops.export_scene.obj(filepath=output_path) 를 사용해야 해.
        bpy.ops.wm.obj_export(
            filepath=output_path,
            export_materials=False,   # 내비메시니까 머티리얼은 필요 없음
            export_normals=False,     # 노멀 데이터도 생략 (필요하다면 True)
            export_uv=False           # UV 생략
        )
        print(f"[Success] Exported: {output_path}")
    except Exception as e:
        print(f"[Error] Failed to export OBJ: {e}")
        sys.exit(1)

if __name__ == "__main__":
    # 블렌더 CLI 자체 인자들과 스크립트용 인자를 구분하기 위해 '--' 이후의 인자만 파싱
    if "--" not  in sys.argv:
        print("Usage: blender -b -P fbx_to_obj.py -- <input_fbx> <output_obj>")
        sys.exit(1)
        
    argv = sys.argv[sys.argv.index("--") + 1:]
    
    if len(argv) < 2:
        print("Error: Missing input or output file paths.")
        sys.exit(1)
        
    input_fbx_path = argv[0]
    output_obj_path = argv[1]
    
    convert_fbx_to_obj(input_fbx_path, output_obj_path)